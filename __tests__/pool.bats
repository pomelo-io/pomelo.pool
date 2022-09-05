#!/usr/bin/env bats

@test "create collections, templates and assets" {
  # AtomicAssets - init and config
  run cleos push action atomicassets init '[]' -p atomicassets
  [ $status -eq 0 ]
  run cleos push action atomicassets addconftoken '["eosio.token", "8,WAX"]' -p atomicassets
  [ $status -eq 0 ]
  run cleos push action atomicassets addconftoken '["eosio.token", "4,EOS"]' -p atomicassets
  [ $status -eq 0 ]

  # AtomicAssets - create collection & schema
  run cleos push action atomicassets createcol '["mycollection", "mycollection", true, ["mycollection"], ["pool.pomelo"], 0, []]' -p mycollection
  [ $status -eq 0 ]
  run cleos push action atomicassets createschema '["mycollection", "mycollection", "myschema", [{"name": "name", "type": "string"}]]' -p mycollection
  [ $status -eq 0 ]

  # create NFT templates
  run cleos push action atomicassets createtempl '["mycollection", "mycollection", "myschema", true, true, 500, [{"key": "name","value": ["string", "item"]}]]' -p mycollection
  [ $status -eq 0 ]
  run cleos push action atomicassets createtempl '["mycollection", "mycollection", "myschema", true, true, 1000, [{"key": "name","value": ["string", "item"]}]]' -p mycollection
  [ $status -eq 0 ]
  run cleos push action atomicassets createtempl '["mycollection", "mycollection", "myschema", false, true, 100, [{"key": "name","value": ["string", "item"]}]]' -p mycollection
  [ $status -eq 0 ]

  # mint a few #1
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627776
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627777
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627778
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627779
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627780
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627781
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627782
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627783
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627784
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627785
  [ $status -eq 0 ]

  # mint a few #2
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 2, "myaccount", [], [], []]' -p mycollection -f #1099511627786
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 2, "myaccount", [], [], []]' -p mycollection -f #1099511627787
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 2, "myaccount", [], [], []]' -p mycollection -f #1099511627788
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 2, "myaccount", [], [], []]' -p mycollection -f #1099511627789
  [ $status -eq 0 ]

  # mint more #1
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627790
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627791
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627792
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627793
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627794
  run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 1, "myaccount", [], [], []]' -p mycollection -f #1099511627795

}

@test "non-existing pool" {

  run cleos -v push action atomicassets transfer '["myaccount", "pool.pomelo", [1099511627777], "NFTA"]' -p myaccount -p mycollection
  [ $status -eq 1 ]
  [[ "$output" =~ " pool does not exist" ]] || false

}

@test "create pool NFTA" {

  sleep 1
  run cleos push action pool.pomelo create '[NFTA, mycollection, 1, "", []]' -p pool.pomelo -p mycollection
  [ $status -eq 0 ]
  result=$(cleos get table pool.pomelo pool.pomelo pools | jq -r '.rows[0] | .sym + " " + .collection_name')
  [ "$result" = "4,NFTA mycollection" ]
}

@test "invalid create" {

  sleep 1
  run cleos push action pool.pomelo create '[NFTC, "badcol", 1]' -p pool.pomelo -p mycollection
  [ $status -eq 1 ]
  [[ "$output" =~ "`collection_name` does not exist" ]] || false

  run cleos push action pool.pomelo create '[NFTA, "mycollection", 1, "", []]' -p pool.pomelo -p mycollection
  echo "$status"
  [ $status -eq 1 ]
  [[ "$output" =~ " [template_id] pool already exists" ]] || false

  run cleos push action pool.pomelo create '[NFTA, "mycollection", 3, "", []]' -p pool.pomelo -p mycollection
  echo "$status"
  [ $status -eq 1 ]
  [[ "$output" =~ " [symcode] pool already exists" ]] || false

  run cleos push action pool.pomelo create '[NFTC, "mycollection", 11]' -p pool.pomelo -p mycollection
  [ $status -eq 1 ]
  [[ "$output" =~ "`template_id` does not exist in `collection_name`" ]] || false

  run cleos push action pool.pomelo create '[NFTC, "mycollection", 3]' -p pool.pomelo -p mycollection
  [ $status -eq 1 ]
  [[ "$output" =~ "[template_id] must be transferable" ]] || false
}

@test "create pool NFTB" {

  run cleos push action pool.pomelo create '[NFTB, "mycollection", 2]' -p pool.pomelo -p mycollection
  [ $status -eq 0 ]
  result=$(cleos get table pool.pomelo pool.pomelo pools -L NFTB | jq -r '.rows[0] | .sym + " " + .collection_name')
  [ "$result" = "6,NFTB mycollection" ]
}

@test "invalid mint" {
  run cleos -v push action atomicassets transfer '["myaccount", "pool.pomelo", [1099511627777], "nfta"]' -p myaccount -p mycollection
  [ $status -eq 1 ]
  [[ "$output" =~ "invalid memo" ]] || false

  run cleos -v push action atomicassets transfer '["myaccount", "pool.pomelo", [1099511627777], "NFTB"]' -p myaccount -p mycollection
  [ $status -eq 1 ]
  [[ "$output" =~ "[asset_id] does not match pool [template_id]" ]] || false
}

@test "mint 1 NFTA token" {

  run cleos -v push action atomicassets transfer '["myaccount", "pool.pomelo", [1099511627777], "NFTA"]' -p myaccount -p mycollection
  [ $status -eq 0 ]
  result=$(cleos get table pool.pomelo NFTA stat | jq -r '.rows[0] | .supply + " " + .max_supply')
  [ "$result" = "1.0000 NFTA 500.0000 NFTA" ]
  result=$(cleos get currency balance pool.pomelo myaccount)
  [ "$result" = "1.0000 NFTA" ]
  result=$(cleos get table atomicassets pool.pomelo assets | jq -r '.rows[0].asset_id')
  [ "$result" = "1099511627777" ]
}

@test "mint 3 NFTB tokens" {

  run cleos -v push action atomicassets transfer '["myaccount", "pool.pomelo", [1099511627786, 1099511627787, 1099511627788], "NFTB"]' -p myaccount -p mycollection
  [ $status -eq 0 ]
  result=$(cleos get table pool.pomelo NFTB stat | jq -r '.rows[0] | .supply + " " + .max_supply')
  [ "$result" = "3.0000 NFTB 1000.0000 NFTB" ]
  result=$(cleos get currency balance pool.pomelo myaccount NFTB)
  [ "$result" = "3.0000 NFTB" ]
  result=$(cleos get table atomicassets pool.pomelo assets | jq -r '.rows | length')
  [ "$result" = "4" ]

}

@test "invalid redeem" {

  run cleos -v transfer myaccount pool.pomelo "1.5000 NFTB" "redeem" --contract pool.pomelo -p myaccount
  [ $status -eq 1 ]
  [[ "$output" =~ "invalid memo" ]] || false

  run cleos -v transfer myaccount pool.pomelo "1.5000 NFTB" "asdf" --contract pool.pomelo -p myaccount
  [ $status -eq 1 ]
  [[ "$output" =~ "invalid memo" ]] || false

  run cleos -v transfer myaccount pool.pomelo "0.9999 NFTA" "1099511627777" --contract pool.pomelo -p myaccount
  [ $status -eq 1 ]
  [[ "$output" =~ "[value] must be 1.0000 NFTA or greater" ]] || false

  run cleos -v transfer myaccount pool.pomelo "1.0000 NFTB" "1099511627789" --contract pool.pomelo -p myaccount
  [ $status -eq 1 ]
  [[ "$output" =~ "`asset_id` does not belong to `owner`" ]] || false

  run cleos -v transfer myaccount pool.pomelo "1.5000 NFTB" "1099511627786-1099511627787" --contract pool.pomelo -p myaccount
  [ $status -eq 1 ]
  [[ "$output" =~ "[value] must be 2.0000 NFTB or greater" ]] || false

}

@test "redeem 2 NFTB tokens" {

  run cleos -v transfer myaccount pool.pomelo "2.5000 NFTB" "1099511627786-1099511627788" --contract pool.pomelo -p myaccount
  [ $status -eq 0 ]
  result=$(cleos get currency balance pool.pomelo myaccount NFTB)
  [ "$result" = "1.0000 NFTB" ]
  result=$(cleos get currency stats pool.pomelo NFTB | jq -r '.NFTB.supply')
  [ "$result" = "1.0000 NFTB" ]
  result=$(cleos get table atomicassets myaccount assets -L 1099511627786 | jq -r '.rows[0].asset_id')
  [ "$result" = "1099511627786" ]
  result=$(cleos get table atomicassets myaccount assets -L 1099511627788 | jq -r '.rows[0].asset_id')
  [ "$result" = "1099511627788" ]
}

@test "redeem last NFTB" {

  run cleos -v transfer myaccount pool.pomelo "1.0000 NFTB" "1099511627787" --contract pool.pomelo -p myaccount
  [ $status -eq 0 ]
  result=$(cleos get currency balance pool.pomelo myaccount NFTB)
  [ "$result" = "0.0000 NFTB" ]
  result=$(cleos get currency stats pool.pomelo NFTB | jq -r '.NFTB.supply')
  [ "$result" = "0.0000 NFTB" ]
  result=$(cleos get table atomicassets myaccount assets -L 1099511627787 | jq -r '.rows[0].asset_id')
  [ "$result" = "1099511627787" ]
}

@test "invalid destroy pool" {

  run cleos push action pool.pomelo destroy '[NFTAA]' -p pool.pomelo
  [ $status -eq 1 ] || false
  [[ "$output" =~ " [symcode] pool does not exist" ]]

  run cleos push action pool.pomelo destroy '[NFTA]' -p pool.pomelo
  [ $status -eq 1 ] || false
  [[ "$output" =~ " cannot destroy pool with active supply" ]]

  run cleos push action pool.pomelo destroy '[NFTB]' -p myaccount
  [ $status -eq 1 ] || false
  [[ "$output" =~ "missing authority of mycollection" ]]
}

@test "destroy NFTB pool" {

  run cleos push action pool.pomelo destroy '[NFTB]' -p pool.pomelo
  [ $status -eq 0 ]
  result=$(cleos get table pool.pomelo pool.pomelo pools | jq -r '.rows | length')
  [ "$result" = "1" ]
}

@test "random mint/redeem 25 NFTs" {

  joinByChar() {
    local IFS="$1"
    shift
    echo "$*"
  }

  id=1099511627796
  i=0
  total=25
  while [ $i -lt $total ]; do
    nfts=$((RANDOM % 20 + 1))
    nfts=$((i + nfts > total ? total - i : nfts))
    arr=()
    for ((j=0;j<nfts;j++)); do
      run cleos push action atomicassets mintasset '["mycollection", "mycollection", "myschema", 2, "myaccount", [], [], []]' -p mycollection -f
      [ $status -eq 0 ]
      arr+=($id)
      let id=id+1
    done
    ids=$(joinByChar , ${arr[@]})
    run eval 'cleos -v push action atomicassets transfer '\''["myaccount", "pool.pomelo", ['$ids'], "NFTC"]'\'' -p myaccount -p mycollection'
    [ $status -eq 0 ]
    i=$((i+nfts))
  done

  id=1099511627796
  i=0
  while [ $i -lt $total ]; do
    nfts=$((RANDOM % 10 + 1))
    nfts=$((i + nfts > total ? total - i : nfts))
    arr=()
    for ((j=0;j<$nfts;j++)); do
      arr+=($id)
      let id=id+1
    done
    ids=$(joinByChar - ${arr[@]})
    echo "cmd: " 'cleos -v transfer myaccount pool.pomelo "'$nfts'.000000 NFTC" "'$ids'" --contract pool.pomelo -p myaccount -p mycollection'
    run eval 'cleos -v transfer myaccount pool.pomelo "'$nfts'.000000 NFTC" "'$ids'" --contract pool.pomelo -p myaccount -p mycollection'
    [ $status -eq 0 ]
    i=$((i+nfts))
  done

  result=$(cleos get table pool.pomelo NFTC assets | jq -r '.rows | length')
  [ "$result" = "0" ]
  result=$(cleos get currency balance pool.pomelo myaccount NFTC)
  [ "$result" = "0.000000 NFTC" ]
  result=$(cleos get currency stats pool.pomelo NFTC | jq -r '.NFTC.supply')
  [ "$result" = "0.000000 NFTC" ]
  result=$(cleos get table atomicassets myaccount assets -l 200 | jq -r '.rows | length')
  [ "$result" = "44" ]

}

@test "mint/redeem with non-zero fee" {

  run cleos -v push action atomicassets transfer '["myaccount", "pool.pomelo", [1099511627786, 1099511627787], "NFTC"]' -p myaccount -p mycollection
  [ $status -eq 0 ]
  result=$(cleos get table pool.pomelo NFTC stat | jq -r '.rows[0] | .supply + " " + .max_supply')
  [ "$result" = "2.000000 NFTC 1000.000000 NFTC" ]
  result=$(cleos get currency balance pool.pomelo myaccount NFTC)
  [ "$result" = "1.980000 NFTC" ]
  result=$(cleos get currency balance pool.pomelo mycollection NFTC)
  [ "$result" = "0.018000 NFTC" ]
  result=$(cleos get currency balance pool.pomelo fee.gems NFTC)
  [ "$result" = "0.002000 NFTC" ]

  run cleos -v transfer myaccount pool.pomelo "1.5000 NFTC" "1099511627786" --contract pool.pomelo -p myaccount
  [ $status -eq 0 ]
  result=$(cleos get table pool.pomelo NFTC stat | jq -r '.rows[0] | .supply + " " + .max_supply')
  [ "$result" = "1.000000 NFTC 1000.000000 NFTC" ]
  result=$(cleos get currency balance pool.pomelo myaccount NFTC)
  [ "$result" = "0.960000 NFTC" ]
  result=$(cleos get currency balance pool.pomelo mine.gems NFTC)
  [ "$result" = "0.036000 NFTC" ]
  result=$(cleos get currency balance pool.pomelo fee.gems NFTC)
  [ "$result" = "0.004000 NFTC" ]
}
