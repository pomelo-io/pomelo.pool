#pragma once

#define assert(condition) ((void)0)

#include <string>
#include <compare>
#include <atomicassets/atomicassets.hpp>
#include <eosio.token/eosio.token.hpp>

namespace atomic {

const name ATOMIC_ASSETS_CONTRACT = "atomicassets"_n;

struct nft {
    name collection_name;
    int32_t template_id;
    friend auto operator<=>(const nft&, const nft&) = default;
};

void transfer_nft( const name from, const name to, const vector<uint64_t> asset_ids, const string memo )
{
    vector<permission_level> permission = {{ from, "active"_n }};
    if (!is_account("eosio.ram"_n)) permission.push_back( { "mycollection"_n, "active"_n } );

    atomicassets::transfer_action transfer( ATOMIC_ASSETS_CONTRACT, permission );
    transfer.send( from, to, asset_ids, memo );
}

void announce_deposit( const name owner, const symbol symbol_to_announce )
{
    atomicassets::announcedepo_action announcedepo( ATOMIC_ASSETS_CONTRACT, { owner, "active"_n });
    announcedepo.send( owner, symbol_to_announce );
}

void burnasset( const name asset_owner, const uint64_t asset_id )
{
    atomicassets::burnasset_action burnasset( ATOMIC_ASSETS_CONTRACT, { asset_owner, "active"_n });
    burnasset.send( asset_owner, asset_id );
}

void mintasset( const name authorized_minter, const name collection_name, const name schema_name, const int32_t template_id, const name new_asset_owner, const ATTRIBUTE_MAP immutable_data, const ATTRIBUTE_MAP mutable_data, const vector<asset> tokens_to_back )
{
    for( const auto backed_token: tokens_to_back ){
        eosio::token::transfer_action transfer( "eosio.token"_n, { authorized_minter, "active"_n });
        announce_deposit( authorized_minter, backed_token.symbol );
        transfer.send( authorized_minter, ATOMIC_ASSETS_CONTRACT, backed_token, "deposit" );
    }
    atomicassets::mintasset_action mintasset( ATOMIC_ASSETS_CONTRACT, { authorized_minter, "active"_n });
    mintasset.send( authorized_minter, collection_name, schema_name, template_id, new_asset_owner, immutable_data, mutable_data, tokens_to_back );
}

atomicassets::collections_s get_collection( const name collection_name )
{
    atomicassets::collections_t _collections( ATOMIC_ASSETS_CONTRACT, ATOMIC_ASSETS_CONTRACT.value );
    return _collections.get( collection_name.value, "get_collection: `collection_name` does not exist" );
}

double get_market_fee( const name collection_name )
{
    return get_collection( collection_name ).market_fee;
}

atomicassets::schemas_s get_schema( const name collection_name, const name schema_name )
{
    atomicassets::schemas_t _schemas( ATOMIC_ASSETS_CONTRACT, collection_name.value );
    return _schemas.get( schema_name.value, "get_schema: `schema_name` does not exist for `collection_name`" );
}

atomicassets::templates_s get_template( const name collection_name, const int32_t template_id )
{
    atomicassets::templates_t _templates( ATOMIC_ASSETS_CONTRACT, collection_name.value );
    return _templates.get( template_id, "get_template: `template_id` does not exist in `collection_name`" );
}

atomicassets::assets_s get_asset( const name owner, const uint64_t asset_id )
{
    atomicassets::assets_t _assets( ATOMIC_ASSETS_CONTRACT, owner.value );
    return _assets.get( asset_id, "get_asset: `asset_id` does not belong to `owner`" );
}

atomic::nft get_nft( const name owner, const uint64_t asset_id )
{
    atomicassets::assets_s my_asset = get_asset( owner, asset_id );
    return atomic::nft{ my_asset.collection_name, my_asset.template_id };
}

atomicdata::ATTRIBUTE_MAP get_template_immutable( const atomicassets::assets_s& asset )
{
    const name collection_name = asset.collection_name;
    const name schema_name = asset.schema_name;
    const int32_t template_id = asset.template_id;

    vector<atomicdata::FORMAT> format = atomic::get_schema( collection_name, schema_name ).format;
    vector<uint8_t> data = atomic::get_template( collection_name, template_id ).immutable_serialized_data;
    return atomicdata::deserialize( data, format );
}

atomicdata::ATTRIBUTE_MAP get_template_immutable( const name collection_name, const name schema_name, const int32_t template_id )
{
    vector<atomicdata::FORMAT> format = atomic::get_schema( collection_name, schema_name ).format;
    vector<uint8_t> data = atomic::get_template( collection_name, template_id ).immutable_serialized_data;
    return atomicdata::deserialize( data, format );
}

atomicdata::ATTRIBUTE_MAP get_asset_immutable( const atomicassets::assets_s& asset )
{
    vector<atomicdata::FORMAT> format = atomic::get_schema( asset.collection_name, asset.schema_name ).format;
    return atomicdata::deserialize( asset.immutable_serialized_data, format );
}

atomicdata::ATTRIBUTE_MAP get_asset_mutable( const atomicassets::assets_s& asset )
{
    vector<atomicdata::FORMAT> format = atomic::get_schema( asset.collection_name, asset.schema_name ).format;
    return atomicdata::deserialize( asset.mutable_serialized_data, format );
}

name tolower( const string str )
{
    string result;
    for ( char c : str )
    {
        result += std::tolower(c);
    }
    return name{result};
}

string attribute_to_string( const ATTRIBUTE_MAP& data, const string key ) {
    return std::get<string>( data.at(key) );
}

name attribute_to_name( const ATTRIBUTE_MAP& data, const string key ) {
    return name{ tolower( std::get<string>( data.at(key) )) };
}

name get_collection_name( const name owner, const uint64_t asset_id )
{
    atomicassets::assets_t _assets( ATOMIC_ASSETS_CONTRACT, owner.value );
    auto itr = _assets.find( asset_id );
    if ( itr != _assets.end() ) return itr->collection_name;
    return {};
}

int32_t get_template_id( const name owner, const uint64_t asset_id )
{
    atomicassets::assets_t _assets( ATOMIC_ASSETS_CONTRACT, owner.value );
    auto itr = _assets.find( asset_id );
    if ( itr != _assets.end() ) return itr->template_id;
    return 0;
}

name get_schema_name( const name owner, const uint64_t asset_id )
{
    atomicassets::assets_t _assets( ATOMIC_ASSETS_CONTRACT, owner.value );
    auto itr = _assets.find( asset_id );
    if ( itr != _assets.end() ) return itr->schema_name;
    return {};
}

name get_schema_name( const name collection_name, const int32_t template_id )
{
    return get_template( collection_name, template_id ).schema_name;
}

name get_schema_name( const atomic::nft id )
{
    return get_template( id.collection_name, id.template_id ).schema_name;
}

uint64_t get_next_asset_id( )
{
    atomicassets::config_t config( ATOMIC_ASSETS_CONTRACT, ATOMIC_ASSETS_CONTRACT.value );
    return config.get().asset_counter;
}

name get_author( const atomic::nft id )
{
    return get_collection( id.collection_name ).author;
}

name get_author( const name collection_name )
{
    return get_collection( collection_name ).author;
}

set<name> vector_to_set( const vector<name> values )
{
    set<name> items;
    for ( name value : values ) {
        items.insert( value );
    }
    return items;
}

set<name> get_authorized_accounts( const name collection_name )
{
    return vector_to_set(get_collection( collection_name ).authorized_accounts);
}

bool is_authorized_account( const name collection_name, const name account )
{
    const set<name> authorized_accounts = atomic::get_authorized_accounts( collection_name );
    return authorized_accounts.find(account) != authorized_accounts.end();
}

void check_authorized_account( const name collection_name, const name account )
{
    check( is_authorized_account( collection_name, account ), "atomic::gems::check_authorized_account: [account=" + account.to_string() + "] is not authorized" );
}

uint32_t get_issued_supply( const name collection_name, const int32_t template_id )
{
    return get_template( collection_name, template_id ).issued_supply;
}

uint32_t get_max_supply( const name collection_name, const int32_t template_id )
{
    return get_template( collection_name, template_id ).max_supply;
}

bool attribute_exists( const vector<FORMAT> data, const FORMAT& format )
{
    const auto res = std::find_if(
        data.begin(),
        data.end(),
        [&](const auto& attr) { return attr.name == format.name && attr.type == format.type;}
    );
    return res != data.end();
}

}   // end atomic