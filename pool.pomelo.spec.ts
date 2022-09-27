import { Name, Asset, TimePointSec } from "@greymass/eosio";
import { Blockchain } from "@proton/vert"

// Vert EOS VM
const blockchain = new Blockchain()

// contracts
const contracts = {
  pool: blockchain.createContract('pool.pomelo', 'pool.pomelo', true),
  atomic: blockchain.createContract('atomicassets', 'external/atomicassets/atomicassets', true),
}

const fake = {
  pool: blockchain.createContract('pool.fake', 'external/eosio.token/eosio.token', true),
  atomic: blockchain.createContract('atomic.fake', 'external/atomicassets/atomicassets', true),
}

// accounts
blockchain.createAccounts('myaccount', 'mycollection', 'pomelo', 'invalid', 'eosio.ram');

interface KV {
  key: string;
  value: string;
}

interface Pools {
  sym: string;
  collection_name: string;
  schema_name: string;
  template_id: number;
  attribute: string;
  values: KV[];
}

const getPool = (symcode: string): Pools => {
  const scope = Name.from('pool.pomelo').value.value;
  const primaryKey = Asset.SymbolCode.from(symcode).value.value;
  return contracts.pool.tables.pools(scope).getTableRow(primaryKey);
}

const getPools = (): Pools[] => {
  const scope = Name.from('pool.pomelo').value.value;
  return contracts.pool.tables.pools(scope).getTableRows();
}

const getBalance = ( account: string, symcode: string ): number => {
  const scope = Name.from(account).value.value;
  const primaryKey = Asset.SymbolCode.from(symcode).value.value;
  const result = contracts.pool.tables.accounts(scope).getTableRow(primaryKey);
  if ( result?.balance ) return Asset.from( result.balance ).units.toNumber();
  return 0;
}

const getSupply = ( symcode: string ): number => {
  const scope = Asset.SymbolCode.from(symcode).value.value;
  const result = contracts.pool.tables.stat(scope).getTableRow(scope);
  if ( result?.supply ) return Asset.from( result.supply ).units.toNumber();
  return 0;
}

const transfer = (from: string, to: string, quantity: string, memo: string) => {
  return contracts.pool.actions.transfer([from, to, quantity, memo]).send(from);
}

const atomic_transfer = (from: string, to: string, asset_ids: number[], memo: string) => {
  return contracts.atomic.actions.transfer([from, to, asset_ids, memo]).send(from);
}

// one-time setup
beforeAll(async () => {
  blockchain.setTime(TimePointSec.from(new Date()));

  // AtomicAssets - init configurations
  await contracts.atomic.actions.init([]).send();

  // AtomicAssets - create collection & schema
  await contracts.atomic.actions.createcol(['mycollection', 'mycollection', true, ['mycollection'], [], 0.08, []]).send('mycollection');
  await contracts.atomic.actions.createschema(['mycollection', 'mycollection', 'myschema', [{"name": "fuel", "type": "uint8"}, {"name": "rarity", "type": "string"}, {"name": "name", "type": "string"}]]).send('mycollection');

  // create NFT item
  await contracts.atomic.actions.createtempl(['mycollection', 'mycollection', 'myschema', true, true, 10000, []]).send('mycollection');
  await contracts.atomic.actions.createtempl(['mycollection', 'mycollection', 'myschema', true, true, 10000, []]).send('mycollection');
  await contracts.atomic.actions.createtempl(['mycollection', 'mycollection', 'myschema', true, true, 10000, []]).send('mycollection');
  await contracts.atomic.actions.createtempl(['mycollection', 'mycollection', 'myschema', true, true, 10000, []]).send('mycollection');
  await contracts.atomic.actions.createtempl(['mycollection', 'mycollection', 'myschema', false, false, 10000, []]).send('mycollection');

  // mint assets
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 1, 'myaccount', [{"key": "fuel", "value": ["uint8", 1]}], [], []]).send('mycollection');
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 1, 'myaccount', [{"key": "fuel", "value": ["uint8", 3]}], [], []]).send('mycollection');
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 1, 'myaccount', [{"key": "fuel", "value": ["uint8", 5]}], [], []]).send('mycollection');
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 2, 'myaccount', [{"key": "rarity", "value": ["string", "Common"]}], [], []]).send('mycollection');
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 2, 'myaccount', [{"key": "rarity", "value": ["string", "Rare"]}], [], []]).send('mycollection');
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 3, 'myaccount', [], [], []]).send('mycollection');
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 3, 'myaccount', [], [], []]).send('mycollection');

  // create fake NFTA token
  await fake.pool.actions.create(["pool.fake", "100.0000 NFTA"]).send();
  await fake.pool.actions.issue(["pool.fake", "100.0000 NFTA", "fake"]).send("pool.fake@active");
});

describe('pool.pomelo', () => {
  test("create - NFTA", async () => {
    const symcode = "NFTA";
    const template_id = 1;
    await contracts.pool.actions.create([symcode, "mycollection", template_id, {"name": "fuel", "type": "uint8"}, [{key: '1', value: 10000}, {key: '3', value: 30000}, {key: '5', value: 50000}]]).send("pool.pomelo");
    expect(getPool(symcode).template_id).toBe(template_id);
  });

  test("create - NFTB", async () => {
    const symcode = "NFTB";
    const template_id = 2;
    await contracts.pool.actions.create([symcode, "mycollection", template_id, {"name": "rarity", "type": "string"}, [{key: 'Common', value: 10000}, {key: 'Rare', value: 30000}]]).send("pool.pomelo");
    expect(getPool(symcode).template_id).toBe(template_id);
  });

  test("create - NFTC", async () => {
    const symcode = "NFTC";
    const template_id = 3;
    await contracts.pool.actions.create([symcode, "mycollection", template_id, {"name": "", "type": ""}, []]).send("pool.pomelo");
    expect(getPool(symcode).template_id).toBe(template_id);
  });

  test("mint - NFTA", async () => {
    await contracts.atomic.actions.transfer(["myaccount", "pool.pomelo", [1099511627776, 1099511627777, 1099511627778], "NFTA"]).send("myaccount");
    expect(getBalance("myaccount", "NFTA")).toBe(90000);
    expect(getSupply("NFTA")).toBe(90000);
  });

  test("mint - NFTB", async () => {
    await contracts.atomic.actions.transfer(["myaccount", "pool.pomelo", [1099511627779, 1099511627780], "NFTB"]).send("myaccount");
    expect(getBalance("myaccount", "NFTB")).toBe(40000);
    expect(getSupply("NFTB")).toBe(40000);
  });

  test("mint - NFTC", async () => {
    await contracts.atomic.actions.transfer(["myaccount", "pool.pomelo", [1099511627781, 1099511627782], "NFTC"]).send("myaccount");
    expect(getBalance("myaccount", "NFTC")).toBe(20000);
    expect(getSupply("NFTC")).toBe(20000);
  });

  test("redeem - NFTA", async () => {
    await contracts.pool.actions.transfer(["myaccount", "pool.pomelo", "9.0000 NFTA", "1099511627776,1099511627777,1099511627778"]).send("myaccount");
    expect(getBalance("myaccount", "NFTA")).toBe(0);
    expect(getSupply("NFTA")).toBe(0);
  });

  test("redeem - NFTB", async () => {
    await contracts.pool.actions.transfer(["myaccount", "pool.pomelo", "4.0000 NFTB", "1099511627779,1099511627780"]).send("myaccount");
    expect(getBalance("myaccount", "NFTB")).toBe(0);
    expect(getSupply("NFTB")).toBe(0);
  });

  test("redeem - NFTC", async () => {
    await contracts.pool.actions.transfer(["myaccount", "pool.pomelo", "2.0000 NFTC", "1099511627781,1099511627782"]).send("myaccount");
    expect(getBalance("myaccount", "NFTC")).toBe(0);
    expect(getSupply("NFTC")).toBe(0);
  });

  test("destroy - NFTB", async () => {
    expect(getPools().length).toBe(3);
    await contracts.pool.actions.destroy(["NFTB"]).send("pool.pomelo");
    expect(getPools().length).toBe(2);
    expect(getSupply("NFTB")).toBe(0);
  })

  // ERRORS
  test("error::create: collection does not exists", async () => {
    const action = contracts.pool.actions.create(["NFTA", "invalid", 1, {"name": "", "type": ""}, []]).send("pool.pomelo");
    await expectToThrow(action, "`template_id` does not exist in `collection_name`");
  });

  test("error::create: attribute type does not match", async () => {
    const action1 = contracts.pool.actions.create(["FOO", "mycollection", 4, {"name": "fuel", "type": "int8"}, []]).send("pool.pomelo");
    await expectToThrow(action1, "[attribute] does not exists");

    const action2 = contracts.pool.actions.create(["FOO", "mycollection", 4, {"name": "invalid", "type": "string"}, []]).send("pool.pomelo");
    await expectToThrow(action2, "[attribute] does not exists");
  });

  test("error::redeem fake token", async () => {
    const action = fake.pool.actions.transfer(["pool.fake", "pool.pomelo", "2.0000 NFTA", "1099511627781,1099511627782"]).send("pool.fake");
    await expectToThrow(action, "invalid quantity");
  });

  test("error::invalid destroy pool", async () => {
    const action1 = contracts.pool.actions.destroy(["NFTAA"]).send("pool.pomelo");
    await expectToThrow(action1, "pool does not exist");

    await contracts.atomic.actions.transfer(["myaccount", "pool.pomelo", [1099511627778], "NFTA"]).send("myaccount");
    const action2 = contracts.pool.actions.destroy(["NFTA"]).send("pool.pomelo");
    await expectToThrow(action2, "cannot destroy pool with active supply");

    const action3 = contracts.pool.actions.destroy(["NFTAA"]).send("myaccount");
    await expectToThrow(action3, "missing required authority pool.pomelo");
  })

  test("error::invalid create", async () => {
    const action1 = contracts.pool.actions.create(["NFTA", "badcol", 1, {"name": "", "type": ""}, []]).send("pool.pomelo");
    await expectToThrow(action1, "get_template: `template_id` does not exist in `collection_name`");

    const action2 = contracts.pool.actions.create(["NFTA", "mycollection", 1, {"name": "", "type": ""}, []]).send("pool.pomelo");
    await expectToThrow(action2, "[template_id] pool already exists");

    const action3 = contracts.pool.actions.create(["NFTA", "mycollection", 3, {"name": "", "type": ""}, []]).send("pool.pomelo");
    await expectToThrow(action3, "[template_id] pool already exists");

    const action4 = contracts.pool.actions.create(["NFTD", "mycollection", 5, {"name": "", "type": ""}, []]).send("pool.pomelo");
    await expectToThrow(action4, "[template_id] must be transferable");
  })

  test("error::invalid redeem", async () => {
    const action1 = transfer("myaccount", "pool.pomelo", "1.0000 NFTA", "redeem");
    await expectToThrow(action1, "invalid memo");

    const action2 = transfer("myaccount", "pool.pomelo", "1.0000 NFTA", "abc");
    await expectToThrow(action2, "invalid memo");

    const action3 = transfer("myaccount", "pool.pomelo", "0.5000 NFTA", "1099511627778");
    await expectToThrow(action3, "requested amount must equal to 5.0000 NFTA");

    const action5 = transfer("myaccount", "pool.pomelo", "2.0000 NFTA", "1099511627778");
    await expectToThrow(action5, "requested amount must equal to 5.0000 NFTA");

    const action4 = transfer("myaccount", "pool.pomelo", "5.0000 NFTA", "1099511627789");
    await expectToThrow(action4, "`asset_id` does not belong to `owner`");
  });

  test("error::invalid mint", async () => {
    const action1 = atomic_transfer("myaccount", "pool.pomelo", [1099511627776], "NFTB");
    await expectToThrow(action1, "pool does not exist");

    // ISSUE TO REPORT: fails on Vert "Sender doesn't own at least one of the provided assets"

    // const action2 = atomic_transfer("myaccount", "pool.pomelo", [1099511627776], "NFTC");
    // await expectToThrow(action2, "[asset_id] does not match pool [template_id]");

    // const action3 = atomic_transfer("myaccount", "pool.pomelo", [1099511627776], "foo");
    // await expectToThrow(action3, "invalid <pool_symbol> memo");

    // const action4 = atomic_transfer("myaccount", "pool.pomelo", [1099511627776], "nfta");
    // await expectToThrow(action4, "invalid <pool_symbol> memo");
  });
});

/**
 * Expect a promise to throw an error with a specific message.
 * @param promise - The promise to await.
 * @param {string} errorMsg - The error message that we expect to see.
 */
 const expectToThrow = async (promise: Promise<any>, errorMsg?: string) => {
  try {
    await promise
    expect(true).toBeFalsy();
  } catch (e: any) {
    if ( errorMsg ) expect(e.message).toMatch(errorMsg)
    else expect(false).toBeFalsy()
  }
}