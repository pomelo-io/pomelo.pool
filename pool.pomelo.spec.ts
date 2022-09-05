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
blockchain.createAccounts('myaccount', 'mycollection', 'pomelo', 'invalid');

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

const getBalance = ( account: string, symcode = "NFTA" ): number => {
  const scope = Name.from(account).value.value;
  const primaryKey = Asset.SymbolCode.from(symcode).value.value;
  const result = contracts.pool.tables.accounts(scope).getTableRow(primaryKey);
  if ( result?.balance ) return Asset.from( result.balance ).units.toNumber();
  return 0;
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

  // mint assets
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 1, 'myaccount', [{"key": "fuel", "value": ["uint8", 1]}], [], []]).send('mycollection');
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 1, 'myaccount', [{"key": "fuel", "value": ["uint8", 3]}], [], []]).send('mycollection');
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 1, 'myaccount', [{"key": "fuel", "value": ["uint8", 5]}], [], []]).send('mycollection');
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 2, 'myaccount', [{"key": "rarity", "value": ["string", "Common"]}], [], []]).send('mycollection');
  await contracts.atomic.actions.mintasset(['mycollection', 'mycollection', 'myschema', 2, 'myaccount', [{"key": "rarity", "value": ["string", "Rare"]}], [], []]).send('mycollection');

  // create fake NFTA token
  await fake.pool.actions.create(["pool.fake", "100.0000 NFTA"]).send();
  await fake.pool.actions.issue(["pool.fake", "100.0000 NFTA", "fake"]).send("pool.fake@active");
});

describe('pool.pomelo', () => {
  it("create", async () => {
    const symcode = "NFTA";
    const collection_name = "mycollection";
    const template_id = 1;
    const auth = [{actor: "mycollection", permission: 'active'}, {actor: "pool.pomelo", permission: 'active'}];
    await contracts.pool.actions.create([symcode, collection_name, template_id, {"name": "fuel", "type": "uint8"}, [{key: '1', value: 10000}, {key: '3', value: 30000}]]).send(auth);
    expect(getPool(symcode).template_id).toBe(template_id);
  });

  it("mint", async () => {
    await contracts.atomic.actions.transfer(["myaccount", "pool.pomelo", [1099511627776, 1099511627777], "NFTA"]).send("myaccount");
    expect(getBalance("myaccount", "NFTA")).toBe(40000);
  });

  // ERRORS
  it("error::create: collection does not exists", async () => {
    const auth = [{actor: "invalid", permission: 'active'}, {actor: "pool.pomelo", permission: 'active'}];
    const action = contracts.pool.actions.create(["NFTA", "invalid", 1, {"name": "", "type": ""}, []]).send(auth);
    await expectToThrow(action, "`collection_name` does not exist");
  });

  it("error::create: attribute type does not match", async () => {
    const auth = [{actor: "mycollection", permission: 'active'}, {actor: "pool.pomelo", permission: 'active'}];
    const action1 = contracts.pool.actions.create(["NFTC", "mycollection", 3, {"name": "fuel", "type": "int8"}, []]).send(auth);
    await expectToThrow(action1, "[attribute] does not exists");

    const action2 = contracts.pool.actions.create(["NFTC", "mycollection", 3, {"name": "invalid", "type": "string"}, []]).send(auth);
    await expectToThrow(action2, "[attribute] does not exists");
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