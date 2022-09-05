#!/bin/bash

# unlock wallet
cleos wallet unlock --password $(cat ~/eosio-wallet/.pass)

# create - core
cleos create account eosio pool.pomelo EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio myaccount EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio mycollection EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

# create - support
cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio atomicassets EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

# deploy
cleos set contract pool.pomelo . pool.pomelo.wasm pool.pomelo.abi
cleos set contract atomicassets include/atomicassets atomicassets.wasm atomicassets.abi

# permissions
cleos set account permission pool.pomelo active --add-code
cleos set account permission atomicassets active --add-code

# required for AtomicAssets in local testing
cleos set account permission mycollection active pool.pomelo --add-code