<h1 class="contract">close</h1>

---
spec_version: "0.2.0"
title: Close Token Balance
summary: 'Close {{nowrap owner}}’s zero quantity balance'
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

{{owner}} agrees to close their zero quantity balance for the {{symbol_to_symbol_code symbol}} token.

RAM will be refunded to the RAM payer of the {{symbol_to_symbol_code symbol}} token balance for {{owner}}.

<h1 class="contract">issue</h1>

---
spec_version: "0.2.0"
title: Issue Tokens into Circulation
summary: 'Issue {{nowrap quantity}} into circulation and transfer into {{nowrap to}}’s account'
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

The token manager agrees to issue {{quantity}} into circulation, and transfer it into {{to}}’s account.

{{#if memo}}There is a memo attached to the transfer stating:
{{memo}}
{{/if}}

If {{to}} does not have a balance for {{asset_to_symbol_code quantity}}, or the token manager does not have a balance for {{asset_to_symbol_code quantity}}, the token manager will be designated as the RAM payer of the {{asset_to_symbol_code quantity}} token balance for {{to}}. As a result, RAM will be deducted from the token manager’s resources to create the necessary records.

This action does not allow the total quantity to exceed the max allowed supply of the token.

<h1 class="contract">open</h1>

---
spec_version: "0.2.0"
title: Open Token Balance
summary: 'Open a zero quantity balance for {{nowrap owner}}'
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

{{ram_payer}} agrees to establish a zero quantity balance for {{owner}} for the {{symbol_to_symbol_code symbol}} token.

If {{owner}} does not have a balance for {{symbol_to_symbol_code symbol}}, {{ram_payer}} will be designated as the RAM payer of the {{symbol_to_symbol_code symbol}} token balance for {{owner}}. As a result, RAM will be deducted from {{ram_payer}}’s resources to create the necessary records.

<h1 class="contract">retire</h1>

---
spec_version: "0.2.0"
title: Remove Tokens from Circulation
summary: 'Remove {{nowrap quantity}} from circulation'
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

The token manager agrees to remove {{quantity}} from circulation, taken from their own account.

{{#if memo}} There is a memo attached to the action stating:
{{memo}}
{{/if}}

<h1 class="contract">transfer</h1>

---
spec_version: "0.2.0"
title: Transfer Tokens
summary: 'Send {{nowrap quantity}} from {{nowrap from}} to {{nowrap to}}'
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

{{from}} agrees to send {{quantity}} to {{to}}.

{{#if memo}}There is a memo attached to the transfer stating:
{{memo}}
{{/if}}

If {{from}} is not already the RAM payer of their {{asset_to_symbol_code quantity}} token balance, {{from}} will be designated as such. As a result, RAM will be deducted from {{from}}’s resources to refund the original RAM payer.

If {{to}} does not have a balance for {{asset_to_symbol_code quantity}}, {{from}} will be designated as the RAM payer of the {{asset_to_symbol_code quantity}} token balance for {{to}}. As a result, RAM will be deducted from {{from}}’s resources to create the necessary records.

<h1 class="contract">create</h1>

---
spec_version: "0.2.0"
title: Create New NFT Token pool
summary: 'Create a new NFT token pool'
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

{{$action.account}} agrees to create a new NFT token pool with symbol {{symcode}} using collection {{collection_name}} & template ID {{template_id}}.

This action will not result any any tokens being issued into circulation.

RAM will deducted from {{$action.account}}’s resources to create the necessary records.

<h1 class="contract">destroy</h1>

---
spec_version: "0.2.0"
title: destroy
summary: Destroy NFT pool
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

<h1 class="contract">rename</h1>

---
spec_version: "0.2.0"
title: rename
summary: Rename NFT pool
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

<h1 class="contract">logmint</h1>

---
spec_version: "0.2.0"
title: logmint
summary: Log mint
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

<h1 class="contract">logredeem</h1>

---
spec_version: "0.2.0"
title: logredeem
summary: Log redeem
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

<h1 class="contract">logcreate</h1>

---
spec_version: "0.2.0"
title: logcreate
summary: Log create
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

<h1 class="contract">logdestroy</h1>

---
spec_version: "0.2.0"
title: logdestroy
summary: Log destroy
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---


<h1 class="contract">logrename</h1>

---
spec_version: "0.2.0"
title: logrename
summary: Log rename
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---

<h1 class="contract">setmax</h1>

---
spec_version: "0.2.0"
title: setmax
summary: Set max supply
icon: https://gateway.pinata.cloud/ipfs/QmNbkDh7ZSkRf7j1peg9YnDoBnJbVg5TMdRfVGYV3hxhhD#b74cf8b3d884f42fffea4bfe7070b3871e1845805c57973a48324af1228ad9cc
---
