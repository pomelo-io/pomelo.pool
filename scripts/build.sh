#!/bin/bash

blanc++ pool.pomelo.cpp -I include

if [ ! -f "./include/atomicassets/atomicassets.wasm" ]; then
    blanc++ ./include/atomicassets/atomicassets.cpp -I include -o include/atomicassets/atomicassets.wasm
fi

if [ ! -f "./include/eosio.token/eosio.token.wasm" ]; then
    blanc++ ./include/eosio.token/eosio.token.cpp -I include -o include/eosio.token/eosio.token.wasm
fi

# unlock wallet & deploy
cleos wallet unlock --password $(cat ~/eosio-wallet/.pass)
cleos set contract pool.pomelo . pool.pomelo.wasm pool.pomelo.abi
