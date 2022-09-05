#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <cmath>

#include <optional>
#include <string>

using namespace eosio;
using namespace std;

class [[eosio::contract("pool.pomelo")]] pool : public eosio::contract {
public:
    using contract::contract;

    const uint8_t TOKEN_PRECISION = 4;
    const int64_t MAX_SUPPLY = 1'000'000'000'0000; // 1M

    // Error messages
    const string ERROR_INVALID_NFT_MEMO = "pool::error: invalid <pool_symbol> memo (ex: NFTA )";
    const string ERROR_INVALID_TOKEN_MEMO = "pool::error: invalid <asset_id> memo (ex: 2199023266689 )";
    const string ERROR_CONFIG_NOT_EXISTS = "pool::error: contract is under maintenance";
    const string ERROR_INVALID_REQUEST_MATCH = "pool::error: quantity amount does not match <asset_id> memo requested";
    const string ERROR_INVALID_QUANITY = "pool::error: invalid quantity";
    const string ERROR_POOL_NOT_EXISTS = "pool::error: pool does not exist";
    const string ERROR_REQUIRE_AUTHOR = "pool::error: must be authorized by `collection_name::author`";
    const string ERROR_INVALID_DUPLICATE = "pool::error: invalid duplicate `asset_ids`";
    const string ERROR_INVALID_PRECISION = "pool::error: invalid precision (ex: `4,NFTA`)";
    const string ERROR_INVALID_MAX_SUPPLY = "pool::error: invalid max supply";
    const string ERROR_INVALID_ASSET_ID_MEMO = "pool::error: invalid memo `<asset_id>` (ex: 1099511627777)";
    const string ERROR_ASSET_NOT_MATCH_TEMPLATE = "pool::error: [asset_id] does not match pool [template_id]";
    const string ERROR_ASSET_NOT_MATCH_COLLECTION = "pool::error: [asset_id] does not match pool [collection_name]";

    /**
     * ## TABLE `pools`
     *
     * | `param`        | `index_position` | `key_type` |
     * |--------------- |------------------|------------|
     * | `bytemplate`   | 3                | i64        |
     *
     * ### params
     *
     * - `{symbol} sym` - (primary key) pool symbol
     * - `{name} collection_name` - AtomicAssets collection name (ex: `mycollection`)
     * - `{name} schema_name` - AtomicAssets schema name (ex: `myschema`)
     * - `{int32_t} template_id` - AtomicAssets template ID (ex: `21881`)
     * - `{atomicdata::FORMAT} attribute` - AtomicAsset attribute key (ex: `{"name": "rarity", "type": "string"}`)
     * - `{map<string, int64_t>} values` - pool asset values
     *
     * ### example
     *
     * ```json
     * {
     *     "sym": "4,NFTA",
     *     "collection_name": "mycollection",
     *     "schema_name": "myschema",
     *     "template_id": 21881,
     *     "attribute": {"name": "rarity", "type": "string"},
     *     "values": [
     *         {"key": "Common", "value": 10000},
     *         {"key": "Rare", "value": 30000},
     *         {"key": "Ultra Rare", "value": 50000}
     *     ]
     * }
     * ```
     */
    struct [[eosio::table("pools")]] pools_row {
        symbol                  sym;
        name                    collection_name;
        name                    schema_name;
        int32_t                 template_id;
        atomicdata::FORMAT      attribute;
        map<string, int64_t>    values;

        uint64_t primary_key() const { return sym.code().raw(); }
        uint64_t by_template() const { return template_id; }
    };
    typedef eosio::multi_index< "pools"_n, pools_row,
        indexed_by<"bytemplate"_n, const_mem_fun<pools_row, uint64_t, &pools_row::by_template>>
    > pools_table;

    /**
     * ## ACTION `create`
     *
     * > Create a new NFT token pool
     *
     * - **authority**: `get_self()`
     *
     * ### checks
     *
     * - `symcode` symbol code must not already exists
     * - `supply` must be finite
     * - `template_id` must be transferable
     * - `template_id` must not already exists
     *
     * ### params
     *
     * - `{symbol_code} symcode` - pool symbol code (ex: `NFTA`)
     * - `{name} collection_name` -  AtomicAssets collection name (ex: `mycollection`)
     * - `{int32_t} template_id` -  AtomicAssets template ID (ex: `21881`)
     * - `{atomicdata::FORMAT} attribute` - AtomicAsset attribute key (ex: `{"name": "rarity", "type": "string"}`)
     * - `{pair<string, int64_t>} [values]` - (optional) pool asset values
     *
     * ### example
     *
     * ```bash
     * $ cleos push action pool.pomelo create '["NFTA", "mycollection", 21882, {"name": "rarity", "type": "string"}, [{"key": "Common", "value": 10000}]]]' -p mycollection -p pool.pomelo
     * ```
     */
    [[eosio::action]]
    void create( const symbol_code symcode, const name collection_name, const int32_t template_id, const atomicdata::FORMAT attribute, const map<string, int64_t> values );

    /**
     * ## ACTION `destroy`
     *
     * > Destroy NFT pool
     *
     * - **authority**: `collection_name::author`
     *
     * ### checks
     *
     * - pool supply must be 0
     *
     * ### params
     *
     * - `{symbol_code} symcode` - pool symbol code (ex: `NFTA`)
     *
     * ### example
     *
     * ```bash
     * $ cleos push action pool.pomelo destroy '["NFTA"]' -p mycollection
     * ```
     */
    [[eosio::action]]
    void destroy( const symbol_code symcode );

    /**
     * ## ACTION `logcreate`
     *
     * > Log create event
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{symbol} sym` - pool symbol (ex: `4,NFTA`)
     * - `{name} collection_name` -  AtomicAssets collection name (ex: `mycollection`)
     * - `{int32_t} template_id` -  AtomicAssets template ID (ex: `21881`)
     * - `{atomicdata::FORMAT} attribute` - AtomicAsset attribute key (ex: `{"name": "rarity", "type": "string"}`)
     * - `{map<string, int64_t>} values` - pool asset values
     *
     * ### Example
     *
     * ```json
     * {
     *     "sym": "4,NFTA",
     *     "collection_name": "mycollection",
     *     "template_id": 21881,
     *     "attribute": {"name": "rarity", "type": "string"},
     *     "values": [
     *         {"key": "Common", "value": 10000},
     *         {"key": "Rare", "value": 30000},
     *         {"key": "Ultra Rare", "value": 50000}
     *     ]
     * }
     * ```
     */
    [[eosio::action]]
    void logcreate( const symbol sym, const name collection_name, const int32_t template_id, const atomicdata::FORMAT attribute, const map<string, int64_t> values );

    /**
     * ## ACTION `logmint`
     *
     * > Log mint event
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{name} owner` - owner account
     * - `{asset} quantity` - quantity minted
     * - `{vector<uint64_t>} asset_ids` - AtomicAsset IDs to mint
     *
     * ### Example
     *
     * ```json
     * {
     *     "owner": "myaccount",
     *     "quantity": "1.0000 NFTA",
     *     "asset_ids": [1099511627776]
     * }
     * ```
     */
    [[eosio::action]]
    void logmint( const name owner, const asset quantity, const vector<uint64_t> asset_ids );

    /**
     * ## ACTION `logredeem`
     *
     * > Log redeem event
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{name} owner` - owner account
     * - `{asset} quantity` - quantity to redeem
     * - `{vector<uint64_t>} asset_ids` - AtomicAsset IDs redeemed
     *
     * ### Example
     *
     * ```json
     * {
     *     "owner": "myaccount",
     *     "quantity": "1.000000 NFTA",
     *     "asset_ids": [1099511627776]
     * }
     * ```
     */
    [[eosio::action]]
    void logredeem( const name owner, const asset quantity, const vector<uint64_t> asset_ids );

    /**
     * ## ACTION `logdestroy`
     *
     * > Log destroy event
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{symbol} symbol` - pool symbol (ex: `4,NFTA`)
     * - `{name} collection_name` -  AtomicAssets collection name (ex: `mycollection`)
     * - `{int32_t} template_id` -  AtomicAssets template ID (ex: `21881`)
     *
     * ### Example
     *
     * ```json
     * {
     *     "symbol": "4,NFTA",
     *     "collection_name": "mycollection",
     *     "template_id": 21881
     * }
     * ```
     */
    [[eosio::action]]
    void logdestroy( const eosio::symbol symbol, const name collection_name, const int32_t template_id );

    /**
     *  This action issues to `to` account a `quantity` of tokens.
     *
     * @param to - the account to issue tokens to, it must be the same as the issuer,
     * @param quantity - the amount of tokens to be issued,
     * @memo - the memo string that accompanies the token issue transaction.
     */
    [[eosio::action]]
    void issue( const name& to, const asset& quantity, const string& memo );

    /**
     * The opposite for create action, if all validations succeed,
     * it debits the statstable.supply amount.
     *
     * @param quantity - the quantity of tokens to retire,
     * @param memo - the memo string to accompany the transaction.
     */
    [[eosio::action]]
    void retire( const asset& quantity, const string& memo );

    /**
     * Allows `from` account to transfer to `to` account the `quantity` tokens.
     * One account is debited and the other is credited with quantity tokens.
     *
     * @param from - the account to transfer from,
     * @param to - the account to be transferred to,
     * @param quantity - the quantity of tokens to be transferred,
     * @param memo - the memo string to accompany the transaction.
     */
    [[eosio::action]]
    void transfer( const name& from, const name& to, const asset& quantity, const string& memo );

    /**
     * Allows `ram_payer` to create an account `owner` with zero balance for
     * token `symbol` at the expense of `ram_payer`.
     *
     * @param owner - the account to be created,
     * @param symbol - the token to be payed with by `ram_payer`,
     * @param ram_payer - the account that supports the cost of this action.
     *
     * More information can be read [here](https://github.com/EOSIO/eosio.contracts/issues/62)
     * and [here](https://github.com/EOSIO/eosio.contracts/issues/61).
     */
    [[eosio::action]]
    void open( const name& owner, const symbol& symbol, const name& ram_payer );

    /**
     * This action is the opposite for open, it closes the account `owner`
     * for token `symbol`.
     *
     * @param owner - the owner account to execute the close action for,
     * @param symbol - the symbol of the token to execute the close action for.
     *
     * @pre The pair of owner plus symbol has to exist otherwise no action is executed,
     * @pre If the pair of owner plus symbol exists, the balance has to be zero.
     */
    [[eosio::action]]
    void close( const name& owner, const symbol& symbol );

    static asset get_supply( const name& token_contract_account, const symbol_code& sym_code )
    {
        stats statstable( token_contract_account, sym_code.raw() );
        const auto& st = statstable.get( sym_code.raw(), "invalid supply symbol code" );
        return st.supply;
    }

    static asset get_balance( const name& token_contract_account, const name& owner, const symbol_code& sym_code )
    {
        accounts accountstable( token_contract_account, owner.value );
        const auto& ac = accountstable.get( sym_code.raw(), "no balance with specified symbol" );
        return ac.balance;
    }

    struct [[eosio::table]] account {
        asset    balance;

        uint64_t primary_key()const { return balance.symbol.code().raw(); }
    };
    typedef eosio::multi_index< "accounts"_n, account > accounts;

    struct [[eosio::table]] currency_stats {
        asset    supply;
        asset    max_supply;
        name     issuer;

        uint64_t primary_key()const { return supply.symbol.code().raw(); }
    };
    typedef eosio::multi_index< "stat"_n, currency_stats > stats;


    [[eosio::on_notify("atomicassets::transfer")]]
    void on_transfer_nft( const name from,
                          const name to,
                          const vector <uint64_t> asset_ids,
                          const string memo );

    [[eosio::on_notify("*::transfer")]]
    void on_transfer_token( const name from,
                            const name to,
                            const asset quantity,
                            const string memo );

    // DEBUG (used to help testing)
    #ifdef DEBUG
    [[eosio::action]]
    void cleartable( const name table_name, const optional<name> scope, const optional<uint64_t> max_rows );
    #endif

    // static actions
    using create_action = eosio::action_wrapper<"create"_n, &pool::create>;
    using destroy_action = eosio::action_wrapper<"destroy"_n, &pool::destroy>;

    // logs
    using logmint_action = eosio::action_wrapper<"logmint"_n, &pool::logmint>;
    using logredeem_action = eosio::action_wrapper<"logredeem"_n, &pool::logredeem>;
    using logcreate_action = eosio::action_wrapper<"logcreate"_n, &pool::logcreate>;
    using logdestroy_action = eosio::action_wrapper<"logdestroy"_n, &pool::logdestroy>;

    // token
    using issue_action = eosio::action_wrapper<"issue"_n, &pool::issue>;
    using retire_action = eosio::action_wrapper<"retire"_n, &pool::retire>;
    using transfer_action = eosio::action_wrapper<"transfer"_n, &pool::transfer>;
    using open_action = eosio::action_wrapper<"open"_n, &pool::open>;
    using close_action = eosio::action_wrapper<"close"_n, &pool::close>;

private:
    // actions
    void mint_tokens( const name owner, const symbol_code symcode, const vector<uint64_t>& asset_ids );
    void redeem_assets( const name owner, const extended_asset ext_in, const vector<uint64_t>& asset_ids );
    void validate_assets( const vector<uint64_t> asset_ids, const pools_row& pool );
    int64_t calculate_issue_amount( const pools_row pool, const vector<uint64_t>& asset_ids );

    // on transfer
    void internal_token_transfer( const name from, const name to, const asset quantity, const string memo );
    vector<uint64_t> parse_memo_asset_ids( const string& memo );
    pool::pools_row get_pool_by_template_id( const int32_t template_id );

    // token
    void create_supply( const name& issuer, const asset& maximum_supply );
    void add_balance( const name& owner, const asset& value, const name& ram_payer );
    void sub_balance( const name& owner, const asset& value );

    // helpers
    void transfer( const name from, const name to, const extended_asset value, const string& memo );
    void issue( const name to, const extended_asset value, const string& memo );
    void retire( const extended_asset value, const string& memo );
    template <typename T>
    void clear_table( T& table );
};
