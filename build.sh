#!/bin/bash

blanc++ pool.pomelo.cpp -I external

if [ ! -f "./external/atomicassets/atomicassets.wasm" ]; then
    blanc++ ./external/atomicassets/atomicassets.cpp -I external -o external/atomicassets/atomicassets.wasm
fi

if [ ! -f "./external/eosio.token/eosio.token.wasm" ]; then
    blanc++ ./external/eosio.token/eosio.token.cpp -I external -o external/eosio.token/eosio.token.wasm
fi
