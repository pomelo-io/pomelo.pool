# #!/bin/bash

# AtomicAssets - init configurations
cleos push action atomicassets init '[]' -p atomicassets

# AtomicAssets - create collection & schema
cleos push action atomicassets createcol '[mycollection, mycollection, true, [mycollection], [], 0.08, []]' -p mycollection
cleos push action atomicassets createschema '[mycollection, mycollection, myschema, [{"name": "name", "type": "string"}]]' -p mycollection

# create NFT item
cleos push action atomicassets createtempl '[mycollection, mycollection, myschema, true, true, 10000, [{"key": "name","value": ["string", "a"]}]]' -p mycollection
cleos push action atomicassets createtempl '[mycollection, mycollection, myschema, true, true, 10000, [{"key": "name","value": ["string", "b"]}]]' -p mycollection

# mint assets
cleos push action atomicassets mintasset '[mycollection, mycollection, myschema, 1, myaccount, [], [], []]' -p mycollection -f
cleos push action atomicassets mintasset '[mycollection, mycollection, myschema, 1, myaccount, [], [], []]' -p mycollection -f
cleos push action atomicassets mintasset '[mycollection, mycollection, myschema, 2, myaccount, [], [], []]' -p mycollection -f
cleos push action atomicassets mintasset '[mycollection, mycollection, myschema, 2, myaccount, [], [], []]' -p mycollection -f
cleos push action atomicassets mintasset '[mycollection, mycollection, myschema, 2, myaccount, [], [], []]' -p mycollection -f

# create
cleos -v push action pool.pomelo create '[NFTA, mycollection, 1, "", []]' -p mycollection -p pool.pomelo
cleos -v push action pool.pomelo create '[NFTB, mycollection, 2, "", []]' -p mycollection -p pool.pomelo

# destroy & create
cleos -v push action pool.pomelo destroy '[NFTB]' -p mycollection -p pool.pomelo
cleos -v push action pool.pomelo create '[NFTB, mycollection, 2, "", []]' -p mycollection -p pool.pomelo -f

# mint
cleos -v push action atomicassets transfer '[myaccount, pool.pomelo, [1099511627776, 1099511627777], "NFTA"]' -p myaccount -p mycollection
cleos -v push action atomicassets transfer '[myaccount, pool.pomelo, [1099511627778, 1099511627779, 1099511627780], "NFTB"]' -p myaccount -p mycollection

# redeem
cleos -v transfer myaccount pool.pomelo "1.0000 NFTA" "1099511627776" --contract pool.pomelo
cleos -v transfer myaccount pool.pomelo "2.0000 NFTB" "1099511627778,1099511627779" --contract pool.pomelo
