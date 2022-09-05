# 🍈 `Pomelo` NFT pool technology [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/facebook/react/blob/main/LICENSE) [![EOSIO Smart Contract](https://github.com/pomelo-io/pomelo.pool/actions/workflows/tests.yml/badge.svg)](https://github.com/pomelo-io/pomelo.pool/actions/workflows/tests.yml)

# Overview

## Contracts

| Chain       | Contract     |
|-------------|--------------|
| EOS         | `pool.pomelo`|
| EOS (dev)   | `d.pool.pomelo`|

## Actions

- Create NFT pool
- Mint `atomicassets` NFT into `eosio.token` token
- Redeem `eosio.token` token for `atomicassets` NFT

### Fees

- **Mint**
    - No fees on mint
- **Redeem**
    - No fee on redeem

## Quickstart

### `USER` (Basic)

**Memo schemas**

| action          | transfer type  | schema                   | example                            |
|-----------------|----------------|--------------------------|------------------------------------|
| mint            | `atomicassets` | `<symbol_code>`          | "NFTA"
| redeem          | `eosio.token`  | `<asset_id>`             | "2199023266689"
| redeem (multi)  | `eosio.token`  | `<asset_id>,<asset_id>`  | "2199023266689,2199023266690"

```bash
# mint NFT
$ cleos push action atomicassets transfer '["myaccount", "pool.pomelo", [1099511627776, 1099511627777], "NFTA"]' -p myaccount
# //=> receive 2.0000 NFTA

# redeem NFT (0% fee)
$ cleos transfer myaccount pool.pomelo "1.0000 NFTA" "1099511627776" --contract pool.pomelo
# //=> receive 1x NFT
```

### `POOL` (Collection Author)

```bash
# create NFT pool
$ cleos push action pool.pomelo create '["NFTA", "mycollection", 21881, "", []]' -p pomelo -p pool.pomelo

# destroy NFT pool (must have zero supply)
$ cleos push action pool.pomelo destroy '["NFTA"]' -p pomelo
```

### `USER` (Defibox)

**Memo schemas**

| action    | schema                          | example                 |
|-----------|---------------------------------|-------------------------|
| swap      |`swap,<min_slippage>,<pair_id>`  | "swap,0,123"
| deposit   |`deposit,<pair_id>`              | "deposit,123"

```bash
# buy pool NFT
cleos transfer myaccount swap.defi "1.0000 EOS" "swap,0,<pair_id>"
# //=> receive NFTA tokens

# sell pool NFT
cleos transfer myaccount swap.defi "1.0000 NFTA" "swap,0,<pair_id>"
# //=> receive EOS tokens

# deposit to Defibox (dual liquidity)
cleos transfer myaccount swap.defi "1.0000 NFTA" --contract pool.pomelo "deposit,<pair_id>"
cleos transfer myaccount swap.defi "1.0000 EOS" --contract eosio.token "deposit,<pair_id>"
cleos push action swap.defi deposit '["myaccount", "<pair_id>"]' -p myaccount
# //=> receive Defibox LP tokens

# withdraw from Defibox (dual liquidity)
cleos transfer myaccount swap.defi "1000000 BOXABC" --contract lptoken.defi ""
# //=> receive 50% NFTA + 50% EOS
```

## Table of Content

- [TABLE `pools`](#table-pools)
- [ACTION `create`](#action-create)
- [ACTION `destroy`](#action-destroy)
- [ACTION `logcreate`](#action-logcreate)
- [ACTION `logmint`](#action-logmint)
- [ACTION `logredeem`](#action-logredeem)
- [ACTION `logdestroy`](#action-logdestroy)

## TABLE `pools`

| `param`        | `index_position` | `key_type` |
|--------------- |------------------|------------|
| `bytemplate`   | 3                | i64        |

### params

- `{symbol} sym` - (primary key) pool symbol
- `{name} collection_name` - AtomicAssets collection name (ex: `mycollection`)
- `{name} schema_name` - AtomicAssets schema name (ex: `myschema`)
- `{int32_t} template_id` - AtomicAssets template ID (ex: `21881`)
- `{atomicdata::FORMAT} attribute` - AtomicAsset attribute key (ex: `rarity`)
- `{map<string, int64_t>} values` - pool asset values

### example

```json
{
    "sym": "4,NFTA",
    "collection_name": "mycollection",
    "schema_name": "myschema",
    "template_id": 21881,
    "attribute": {"name": "rarity", "type": "string"},
    "values": [
        {"key": "Common", "value": 10000},
        {"key": "Rare", "value": 30000},
        {"key": "Ultra Rare", "value": 50000}
    ]
}
```

## ACTION `create`

> Create a new NFT token pool

- **authority**: `collection_name::author`  & `get_self()`

### checks

- `symcode` symbol code must not already exists
- `supply` must be finite
- `template_id` must be transferable
- `template_id` must not already exists

### params

- `{symbol_code} symcode` - pool symbol code (ex: `NFTA`)
- `{name} collection_name` -  AtomicAssets collection name (ex: `mycollection`)
- `{int32_t} template_id` -  AtomicAssets template ID (ex: `21881`)
- `{atomicdata::FORMAT} attribute` - AtomicAsset attribute key (ex: `{"name": "rarity", "type": "string"}`)
- `{pair<string, int64_t>} [values]` - (optional) pool asset values

### example

```bash
$ cleos push action pool.pomelo create '["NFTA", "mycollection", 21882, {"name": "rarity", "type": "string"}, [{"key": "Common", "value": 10000}]]]' -p mycollection -p pool.pomelo
```

## ACTION `destroy`

> destroy NFT pool

- **authority**: `collection_name::author`

### checks

- pool supply must be 0

### params

- `{symbol_code} symcode` - pool symbol code (ex: `NFTA`)

### example

```bash
$ cleos push action pool.pomelo destroy '["NFTA"]' -p mycollection
```

## ACTION `logcreate`

> Log create event

- **authority**: `get_self()`

### params

- `{symbol} sym` - pool symbol (ex: `4,NFTA`)
- `{name} collection_name` -  AtomicAssets collection name (ex: `mycollection`)
- `{int32_t} template_id` -  AtomicAssets template ID (ex: `21881`)
- `{atomicdata::FORMAT} attribute` - AtomicAsset attribute key (ex: `{"name": "rarity", "type": "string"}`)
- `{map<string, int64_t>} values` - pool asset values

### Example

```json
{
    "sym": "4,NFTA",
    "collection_name": "mycollection",
    "template_id": 21881,
    "attribute": {"name": "rarity", "type": "string"},
    "values": [
        {"key": "Common", "value": 10000},
        {"key": "Rare", "value": 30000},
        {"key": "Ultra Rare", "value": 50000}
    ]
}
```

## ACTION `logmint`

> Log mint event

- **authority**: `get_self()`

### params

- `{name} owner` - owner account
- `{asset} quantity` - quantity minted
- `{vector<uint64_t>} asset_ids` - AtomicAsset IDs to mint

### Example

```json
{
    "owner": "myaccount",
    "quantity": "1.0000 NFTA",
    "asset_ids": [1099511627776]
}
```

## ACTION `logredeem`

> Log redeem event

- **authority**: `get_self()`

### params

- `{name} owner` - owner account
- `{asset} quantity` - quantity to redeem
- `{vector<uint64_t>} asset_ids` - AtomicAsset IDs redeemed

### Example

```json
{
    "owner": "myaccount",
    "quantity": "1.0000 NFTA",
    "asset_ids": [1099511627776]
}
```

## ACTION `logdestroy`

> Log destroy event

- **authority**: `get_self()`

### params

- `{symbol} symbol` - pool symbol (ex: `4,NFTA`)
- `{name} collection_name` -  AtomicAssets collection name (ex: `mycollection`)
- `{int32_t} template_id` -  AtomicAssets template ID (ex: `21881`)

### Example

```json
{
    "symbol": "4,NFTA",
    "collection_name": "mycollection",
    "template_id": 21881
}
```