#include <gems/atomic.hpp>
#include <gems/utils.hpp>

#include "pool.pomelo.hpp"
#include "src/logs.cpp"
#include "src/token.cpp"

// DEBUG (used to help testing)
#ifdef DEBUG
#include "src/debug.cpp"
#endif

[[eosio::on_notify("atomicassets::transfer")]]
void pool::on_transfer_nft( const name from,
                            const name to,
                            const vector <uint64_t> asset_ids,
                            const string memo )
{
    // ISSUE TO REPORT: fails on Vert "missing required authority <from>"
    // require_auth( from );

    // ignore outgoing transfer from self
    if ( from == get_self() || to != get_self() ) return;

    // validate memo
    const symbol_code symcode = utils::parse_symbol_code( memo );
    check( symcode.raw(), ERROR_INVALID_NFT_MEMO );

    // mint - memo required (ex: "<symcode>")
    mint_tokens( from, symcode, asset_ids );
}

[[eosio::on_notify("*::transfer")]]
void pool::on_transfer_token( const name from,
                              const name to,
                              const asset quantity,
                              const string memo )
{
    // ISSUE TO REPORT: fails on Vert "missing required authority <from>"
    // require_auth( from );

    // ignore outgoing transfer from self
    if ( from == get_self() || to != get_self() || from == "eosio.ram"_n ) return;


    // validate quantity
    check( false, ERROR_INVALID_QUANITY );
}

void pool::internal_token_transfer( const name from, const name to, const asset quantity, const string memo )
{
    require_auth( from );

    // ignore outgoing transfer from self
    if ( from == get_self() || to != get_self() ) return;

    // validate quantity
    const extended_asset ext_in = { quantity, get_first_receiver() };
    check( get_first_receiver() == get_self(), ERROR_INVALID_QUANITY );
    check( quantity.symbol.precision() == TOKEN_PRECISION, ERROR_INVALID_QUANITY );

    // validate memo
    const vector<uint64_t> asset_ids = parse_memo_asset_ids( memo );
    const int size = asset_ids.size();
    check( size > 0, ERROR_INVALID_TOKEN_MEMO );

    // redeem - memo required ( ex: "<asset_id>,<asset_id>" )
    redeem_assets( from, ext_in, asset_ids );
}

pool::pools_row pool::get_pool_by_template_id( const int32_t template_id )
{
    pools_table _pools( get_self(), get_self().value );
    const auto index = _pools.get_index<"bytemplate"_n>();
    const auto itr = index.find( template_id );
    check( itr != index.end(), "pool::get_pool_by_template_id: [template_id=" + to_string(template_id) + "] does not exists");
    return *itr;
}

void pool::mint_tokens( const name owner, const symbol_code symcode, const vector<uint64_t>& asset_ids )
{
    pools_table _pools( get_self(), get_self().value );

    // validate input
    auto pool = _pools.get( symcode.raw(), ERROR_POOL_NOT_EXISTS.c_str() );
    const symbol sym = pool.sym;
    const extended_symbol ext_sym = {sym, get_self()};
    validate_assets( asset_ids, pool );

    // calculate mint amounts
    const int64_t issue_amount = calculate_issue_amount( pool, asset_ids );

    // transfer to owner
    issue( get_self(), { issue_amount, ext_sym }, "mint" );
    transfer( get_self(), owner, { issue_amount, ext_sym }, "mint" );

    // logging
    logmint_action logmint( get_self(), { get_self(), "active"_n });
    logmint.send( owner, asset{ issue_amount, sym }, asset_ids );
}

int64_t pool::calculate_issue_amount( const pools_row pool, const vector<uint64_t>& asset_ids )
{
    // no attribute
    const uint8_t precision = pool.sym.precision();
    const int64_t issue_amount = asset_ids.size() * pow( 10, precision );
    if ( !pool.attribute.name.length() ) return issue_amount;

    // calculate using attribute
    int64_t total_issue_amount = 0;
    for ( const uint64_t asset_id : asset_ids ) {
        const atomicdata::ATTRIBUTE_MAP immutable = atomic::get_asset_immutable(atomic::get_asset( get_self(), asset_id ));
        const string value = atomic::attribute_to_string(immutable, pool.attribute);

        // attribute doesn't exists, fallback to using standard 1:1 ratio
        if ( pool.values.find(value) == pool.values.end() ) {
            total_issue_amount += pow( 10, precision );
            continue;
        }
        // use defined amount values in pool settings
        total_issue_amount += pool.values.at(value);
    }

    return total_issue_amount;
}

void pool::redeem_assets( const name owner, const extended_asset ext_in, const vector<uint64_t>& asset_ids )
{
    pools_table _pools( get_self(), get_self().value );

    // input
    const asset quantity = ext_in.quantity;
    const symbol_code symcode = quantity.symbol.code();

    // validate input
    auto pool = _pools.get( symcode.raw(), ERROR_POOL_NOT_EXISTS.c_str() );
    const extended_symbol ext_sym = { pool.sym, get_self() };
    check( ext_sym == ext_in.get_extended_symbol(), ERROR_INVALID_QUANITY );
    validate_assets( asset_ids, pool );

    // validate quantity input
    const int64_t request_amount = calculate_issue_amount( pool, asset_ids );
    check( quantity.amount == request_amount, "pool::redeem_assets: requested amount must equal to " + asset{ request_amount, quantity.symbol }.to_string() );

    // retire incoming token
    retire( ext_in, "redeem");

    // transfer NFT to owner
    atomic::transfer_nft( get_self(), owner, asset_ids, "redeem");

    // logging
    logredeem_action logredeem( get_self(), { get_self(), "active"_n });
    logredeem.send( owner, quantity, asset_ids );
}

void pool::validate_assets(const vector<uint64_t> asset_ids, const pools_row& pool )
{
    for ( const uint64_t & asset_id : asset_ids ) {
        const auto asset = atomic::get_asset( get_self(), asset_id );
        check( asset.template_id == pool.template_id, ERROR_ASSET_NOT_MATCH_TEMPLATE );
        check( asset.collection_name == pool.collection_name, ERROR_ASSET_NOT_MATCH_COLLECTION );
    }
}

[[eosio::action]]
void pool::create( const symbol_code symcode, const name collection_name, const int32_t template_id, const atomicdata::FORMAT attribute, const map<string, int64_t> values )
{
    stats _stats( get_self(), symcode.raw() );
    pools_table _pools( get_self(), get_self().value );

    // // Must be created by collection author
    // const name author = atomic::get_author( collection_name );
    // require_auth( author );

    // restrict creation authority
    require_auth( get_self() );

    // verify template exists and transferable
    const auto& mytemplate = atomic::get_template( collection_name, template_id );
    check( mytemplate.transferable, "pool::create: [template_id] must be transferable");

    // validate attribute if exists
    if ( attribute.name.length() ) {
        vector<atomicdata::FORMAT> format = atomic::get_schema( collection_name, mytemplate.schema_name ).format;
        check( atomic::attribute_exists(format, attribute), "pool::create: [attribute] does not exists");
    }

    // extract max values from attribute (default 10000)
    int64_t max_value = pow( 10, TOKEN_PRECISION );
    for ( const auto& [key, value] : values ) {
        max_value = max( max_value, value );
    }

    // verify wrap for this template doesn't already exist
    const auto index = _pools.get_index<"bytemplate"_n>();
    const auto itr = index.find( template_id );
    check( itr == index.end(), "pool::create: [template_id] pool already exists");
    check( _pools.find( symcode.raw()) == _pools.end(), "pool::create: [symcode] pool already exists");
    check( _stats.find( symcode.raw()) == _stats.end(), "pool::create: [symcode] token supply already exists");

    // validate max supply (enforce max supply)
    const symbol sym = { symcode, TOKEN_PRECISION };
    int64_t max_supply_amount = mytemplate.max_supply * max_value;
    if ( max_supply_amount == 0 || max_supply_amount > MAX_SUPPLY ) max_supply_amount = MAX_SUPPLY;
    // check( max_supply_amount > 0, ERROR_INVALID_MAX_SUPPLY );

    // create pool
    _pools.emplace( get_self(), [&]( auto& row ) {
        row.sym = sym;
        row.collection_name = collection_name;
        row.schema_name = mytemplate.schema_name;
        row.template_id = template_id;
        row.attribute = attribute;
        row.values = values;
    });

    // create NFT token supply
    create_supply( get_self(), { max_supply_amount, sym } );

    // logging
    logcreate_action logcreate( get_self(), { get_self(), "active"_n });
    logcreate.send( sym, collection_name, template_id, attribute, values );
}

[[eosio::action]]
void pool::setmax( const symbol_code symcode )
{
    stats _stats( get_self(), symcode.raw() );
    pools_table _pools( get_self(), get_self().value );

    // restrict creation authority
    require_auth( get_self() );

    auto & stats = _stats.get( symcode.raw(), ERROR_POOL_NOT_EXISTS.c_str() );
    auto & pool = _pools.get( symcode.raw(), ERROR_POOL_NOT_EXISTS.c_str() );
    const auto& mytemplate = atomic::get_template( pool.collection_name, pool.template_id );

    // extract max values from attribute (default 10000)
    int64_t max_value = pow( 10, TOKEN_PRECISION );
    for ( const auto& [key, value] : pool.values ) {
        max_value = max( max_value, value );
    }

    // set max supply
    int64_t max_supply_amount = mytemplate.max_supply * max_value;
    _stats.modify( stats, get_self(), [&]( auto& row ) {
        row.max_supply.amount = max_supply_amount;
    });
}

[[eosio::action]]
void pool::destroy( const symbol_code symcode )
{
    // authorized
    require_auth( get_self() );

    pools_table _pools( get_self(), get_self().value );
    stats _stats( get_self(), symcode.raw() );
    const auto & pools = _pools.get( symcode.raw(), ERROR_POOL_NOT_EXISTS.c_str() );

    // logging
    logdestroy_action logdestroy( get_self(), { get_self(), "active"_n });
    logdestroy.send( pools.sym, pools.collection_name, pools.template_id );

    // must not have any supply to be erased
    const asset supply = token::get_supply( get_self(), symcode );
    check( supply.amount == 0, "pool::destroy: cannot destroy pool with active supply" );
    _pools.erase( pools );

    // destroy symbol
   const auto& st = _stats.get( symcode.raw(), "symbol does not exist" );
   _stats.erase( st );
}

[[eosio::action]]
void pool::rename( const symbol_code symcode, const symbol_code new_symcode, const vector<name> accounts )
{
    // authorized
    require_auth( get_self() );

    pools_table _pools( get_self(), get_self().value );
    stats _stats( get_self(), symcode.raw() );
    stats _stats_new( get_self(), new_symcode.raw() );

    // pool info
    const auto & pools = _pools.get( symcode.raw(), ERROR_POOL_NOT_EXISTS.c_str() );
    const symbol sym = pools.sym;
    const name collection_name = pools.collection_name;
    const name schema_name = pools.schema_name;
    const int32_t template_id = pools.template_id;
    const atomicdata::FORMAT attribute = pools.attribute;
    const map<string, int64_t> values = pools.values;

    // supply info
    const auto& st = _stats.get( symcode.raw(), "symbol does not exist" );
    const asset supply = st.supply;
    const asset max_supply = st.max_supply;
    const name issuer = st.issuer;

    // rename pool symbol
    _pools.erase( pools );
    _stats.erase( st );

    // replace new primary keys
    _pools.emplace( get_self(), [&]( auto& row ) {
        row.sym = symbol{new_symcode, sym.precision()};
        row.collection_name = collection_name;
        row.schema_name = schema_name;
        row.template_id = template_id;
        row.attribute = attribute;
        row.values = values;
    });

    _stats_new.emplace( get_self(), [&]( auto& row ) {
        row.supply = {supply.amount, symbol{new_symcode, sym.precision()}};
        row.max_supply = {max_supply.amount, symbol{new_symcode, sym.precision()}};
        row.issuer = issuer;
    });

    // rename accounts balance token symbol
    int64_t total_accounts_amount = 0;
    for ( const name& account : accounts ) {
        accounts_table _accounts( get_self(), account.value );
        const auto& accnt = _accounts.get( symcode.raw(), "symbol does not exist for account" );
        const int64_t balance = accnt.balance.amount;
        _accounts.erase( accnt );
        _accounts.emplace( get_self(), [&]( auto& row ) {
            row.balance = {balance, {new_symcode, pools.sym.precision()}};
        });
        total_accounts_amount += balance;
    }
    check( supply.amount == total_accounts_amount, "pool::rename: renaming pool must match accounts with active supply" );
}

// Memo schemas
// ============
// Single: `<asset_id>` (ex: "1099511627777")
// Multiple: `<asset_id>,<asset_id>` (ex: "1099511627777,1099511627778")
vector<uint64_t> pool::parse_memo_asset_ids( const string& memo )
{
    set<uint64_t> duplicates;
    vector<uint64_t> asset_ids;
    for ( const string& str : utils::split(memo, ",") ) {
        const uint64_t asset_id = std::strtoull( str.c_str(), nullptr, 10 );
        check( asset_id, ERROR_INVALID_ASSET_ID_MEMO );
        asset_ids.push_back( asset_id );
        check( !duplicates.count( asset_id ), ERROR_INVALID_DUPLICATE );
        duplicates.insert( asset_id );
    }
    return asset_ids;
}