# Round 7 actor command tables

Tags: #data #studio #monsters

Decoded all eleven command tables installed by the monster initializers and
shared callback defaults: 336 bytes round-trip exactly, including Octopus
commands 6–8 as null entries. Sources now live in `data/actor_commands/`.

| Table | Start | End | Entries |
|---|---|---|---:|
| Bat | 08605C74 | 08605C94 | 8 |
| Bee | 08605DB8 | 08605DD4 | 7 |
| Ghost | 08605E14 | 08605E34 | 8 |
| Lun | 08605E78 | 08605E94 | 7 |
| Crow | 08605EC4 | 08605EE0 | 7 |
| Spider | 08605F34 | 08605F50 | 7 |
| Vorn | 08606038 | 08606058 | 8 |
| Skeleton | 086060FC | 08606120 | 9 |
| Octopus | 0860635C | 08606384 | 10 |
| Ivy | 08606400 | 0860641C | 7 |
| Common defaults | 086108B0 | 086108C8 | 6 |

The initializer stores at actor +0x250 identify each table; adjacent data labels
bound its extent. All eleven occurrences of the six common handler pointers
in the ROM are covered. Shared `MonsterInit_InitializeCallbackDefaults` installs
086108B0 before the species overrides in Spider, Vorn, Skeleton, Octopus and Ivy.
Ghost installs its own table directly; the other initializers retain the common
table or use the Bat, Bee, Lun and Crow overrides.

Studio now gives each table a separate ROM block. The surrounding data gaps also
retain separate intervals, so their aggregate sizes cannot overlap a decoded
table. The actor-table and callback-codec test suites pass; the ROM comparison
checks each CSV and preserves all nonzero entries as relocatable pointers.
