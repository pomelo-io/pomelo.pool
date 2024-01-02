void pool::transfer( const name from, const name to, const extended_asset value, const string& memo )
{
    eosio::token::transfer_action transfer( value.contract, { from, "active"_n });
    transfer.send( from, to, value.quantity, memo );
}

void pool::issue( const name to, const extended_asset value, const string& memo )
{
    eosio::token::issue_action issue( value.contract, { get_self(), "active"_n });
    issue.send( to, value.quantity, memo );
}

void pool::retire( const extended_asset value, const string& memo )
{
    eosio::token::retire_action retire( value.contract, { get_self(), "active"_n });
    retire.send( value.quantity, memo );
}

[[eosio::action]]
void pool::logmint( const name owner, const asset quantity, const vector<uint64_t> asset_ids )
{
    require_auth( get_self() );
}

[[eosio::action]]
void pool::logredeem( const name owner, const asset quantity, const vector<uint64_t> asset_ids )
{
    require_auth( get_self() );
}

[[eosio::action]]
void pool::logcreate( const symbol sym, const name collection_name, const int32_t template_id, const atomicdata::FORMAT attribute, const map<string, int64_t> values )
{
    require_auth( get_self() );
}

[[eosio::action]]
void pool::logdestroy( const symbol sym, const name collection_name, const int32_t template_id )
{
    require_auth( get_self() );
}

[[eosio::action]]
void pool::logrename( const eosio::symbol symbol, const name collection_name, const int32_t template_id, const symbol_code new_symcode, const vector<name> accounts )
{
    require_auth( get_self() );
}