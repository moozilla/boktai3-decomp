# Source layout

Tags: #tooling #process

The C source is arranged by what each piece of code *is in the game*, the way
zeldaret's tmc and pret's pokeemerald arrange theirs. Go to the category you
would guess as a player, then to the object by its game name. Identities were
checked against the running game. The [layout decisions](SOURCE_LAYOUT_DECISIONS.md)
list the evidence and the names still awaiting confirmation.

## The map

| Directory | What is in it |
|---|---|
| `src/player/` | Django, cut along his ROM ranges: core, movement, interaction (coffin chain, pushing), motions, melee attack, the Gun Del Sol, control, stats, state, cutscene, appearance, lock-on targets, effects, clone |
| `src/monsters/` | Every Monster Book entry that is not a boss: one file or directory per monster |
| `src/bosses/` | The Immortals and their forms: Hresvelgr, Nidhoggr (ship and duel), Ratatosk, Vanargand (arena, head in the bike chase), Sabata, and the Count |
| `src/npc/` | Townspeople, story characters, cutscene actors, Sabata's rider in the ending |
| `src/objects/` | Things placed in the world: coffin, Pile Driver, Warp Circle, Solar Stand, deck cannons, Solar Roots, crates, switches, items, hazards, markers |
| `src/minigames/` | The Coffin Bike, the Dark Generator, the result game |
| `src/effects/` | Visual effects: palette, particles, screen effects, projectiles, bursts, orbits, sprite effects, shadows, weather |
| `src/ui/` | HUD, dialogue, pause menu, town services and the town map, the world map, the books, title screens, results |
| `src/link/` | Two-player play: cable (`serial`), wireless, the link menus, and the link-play Django (`link/player`) |
| `src/system/` | The cartridge and what is saved: solar sensor, clock and sun schedule, saving, game state, inventory data |
| `src/scene/` | The world simulation everything shares: the actor framework, collision, terrain and paths, gameplay areas, camera, scene lifecycle |
| `src/script/` | The GCL script interpreter (Metal Gear Solid's scripting language) and its runtime |
| `src/engine/` | Boot, tasks, heap, interrupts, video (backgrounds, sprites, the animated sprite, palettes, text tiles), math, game-side sound |
| `src/lib/` | Third-party libraries in upstream form: m4a (sound driver), rfu (wireless), rtc, eeprom, libgcc, libc, libm |

## Conventions

* `src/<category>/<object>/` for an object with three or more files;
  `src/<category>/<object>.c` for a one-file object; a two-file object's files
  sit in the category. Five two-file objects keep a directory because their
  file names do not say what they belong to: `objects/coffin/`,
  `objects/bat_curtain/`, `objects/dungeon_prop/`, `ui/save/`, `ui/game_over/`.
* A `<group>/` level only where a category has families
  (`effects/particles/`, `ui/pause/`, `ui/town/`, `link/wireless/`,
  `monsters/cannons/`, `objects/markers/`, `scene/actor/`, `player/effects/`,
  `engine/video/`). Never more than three directory levels below `src/`.
* A directory does not repeat its parent's word (`link/panel`, not
  `link/link_panel`; `ui/pause/menu`, not `ui/pause/pause_menu`).
* Lowercase snake_case nouns, the game's own name where it has one (Monster Book
  names, `gcl/names/chara.csv`, text the screens show), no numbers, no hex.
* Each object is one directory, and every directory is made of whole ROM
  ranges: one contiguous range where possible, a few at most (Player's 18
  directories hold 22 ranges. See the range rules in `symbols/source_dirs.csv`). That lets each range
  become one unit file.
* Function names are `Object_Action` with the object's game name
  (`PileDriver_`, `VanargandHead_`, `Sabata_`, `Shop_`), never a registry key,
  slot or address. One-function files are named after their function
  (`Hresvelgr_Create.c`); an unnamed function keeps `sub_XXXXXXXX.c`.
* Headers sit beside their object under the mirrored path in `include/`, at
  most two directories deep (`#include "monsters/ghoul.h"`,
  `#include "objects/pile_driver/scene_panel_rig.h"`); a three-level source
  directory keeps its headers one level up (`src/ui/pause/map/` →
  `include/ui/pause/`). `global.h` and `debug.h` are at the root. The
  scene-actor framework's shared types are grouped by topic:
  `include/scene/actor/` (registry, state, components, init, scripts),
  `include/scene/collision/` (contacts and collision responses),
  `include/scene/motion/` (routes, chase, turning, terrain movement). Engine-wide
  headers: `include/engine/` (memory, tasks, input, RAM), `include/engine/video/`
  (display sequences, palettes, render), `include/engine/math/`.

## Where is it?

| You are looking for | Go to |
|---|---|
| Django: create, update, destroy, per-frame update | `src/player/core/` |
| Walking, turning, slopes, forces, camera offsets | `src/player/movement/` |
| Dragging the coffin, pushing blocks | `src/player/interaction/` |
| Motions: paralysis, death, form changes, special casts (Spiral Burst) | `src/player/motions/` |
| Swords: Gradius, Estoc, Kopis, Zweihander, Kagerou | `src/player/attack/` |
| The Gun Del Sol: frames, shots, charge, Tempest, Rockbuster | `src/player/gun/` |
| Input and action selection | `src/player/control/` |
| Life, energy, resistance, equipment values | `src/player/stats/` |
| Scripted position requests, stun, mode flags | `src/player/state/` |
| Django in cutscenes (cues, script commands) | `src/player/cutscene/` |
| Django's model, palettes, visuals | `src/player/appearance/` |
| Lock-on targets; barrier, burst, attack effects; clones | `src/player/target_registry/`, `src/player/effects/`, `src/player/clone.c` |
| Ghoul family (ボク) | `src/monsters/ghoul.c`, `ghoul_states.c` |
| Bat, Bee, Slime, Ghost, Crow, Spider, Mimic, Octopus, Dog (Hound, Hot Dog, Ice Dog), Lun | `src/monsters/<name>.c` |
| Skeleton, Golem, Mummy, Vorn, Ivy, Sword, Axe, Ooze, Screamer, Poison Hand, Dragon Puppy | `src/monsters/<name>/` |
| Bronze, Iron, Flame, Frost, Cloud, Earth, Dual Armor, Black Knight | `src/monsters/armor/` |
| ARMOR (the stationary suit, Book #29) and STATUE (#28) | `src/monsters/suit_of_armor/`, `src/monsters/statue/` |
| Garmr Statue, Muspell Head, Popbud, Poisoner and Perplexer, Centipede | `src/monsters/garmr_statue/`, `muspell_head/`, `popbud/`, `pollen_flowers/`, `centipede/` |
| Vulcan, Wind and Plasma Cannons | `src/monsters/cannons/{vulcan,wind,plasma}/` |
| Hresvelgr (Ancient Tree) | `src/bosses/hresvelgr/` (its Solar Roots: `src/objects/solar_roots/`) |
| Nidhoggr: iron dragon and ship, and the duel inside | `src/bosses/nidhoggr/` |
| Ratatosk (Dark Castle) | `src/bosses/ratatosk/` |
| Vanargand, including his head in the bike chase | `src/bosses/vanargand/` |
| Sabata (White Forest and rematch), his rider in the ending | `src/bosses/sabata/`, `src/npc/sabata_rider/` |
| The Count and his bat swarm | `src/bosses/count/` |
| The Pile Driver and purification | `src/objects/pile_driver/` |
| The coffin | `src/objects/coffin/` |
| The Coffin Bike (Vector Coffin), the bike battle | `src/minigames/coffin_bike/` |
| The Dark Generator (Dark Loan work) | `src/minigames/dark_generator.c` |
| Warp Circle, Solar Stand, deck cannons, wooden crate, switches, Bat Curtain | `src/objects/<name>/` |
| Treasure chests and dropped items | `src/objects/items/` |
| Townspeople, story characters, cutscene actors | `src/npc/` |
| The solar sensor, solar level, calibration | `src/system/solar/` |
| Clock, sunrise and sunset, region setting | `src/system/clock/` (RTC driver `src/lib/rtc/`) |
| Saving | `src/system/save/` (EEPROM `src/lib/eeprom/`), save screen `src/ui/save/` |
| Story flags, game state, Solar Fruit; item data | `src/system/game_state/`, `src/system/inventory/` |
| Music and sound effects | `src/engine/sound/`, driver `src/lib/m4a/` |
| The script interpreter (GCL), cutscene runner | `src/script/script_virtual_machine.c`, `src/script/demo_manager.c`; scripts in `gcl/` |
| Dialogue boxes and text commands | `src/ui/dialogue/` |
| HUD and damage numbers | `src/ui/hud/` |
| Pause menu, inventory, map, Status page, Dark Card sequences | `src/ui/pause/{menu,inventory,map,status,menu_sequences}/` |
| Shops, Solar Bank, Dark Loan, Storage, the San Miguel map | `src/ui/town/` (`shop/`, `solar_bank/`, `dark_loan.c`, `storage/`, `town_map.c`) |
| The world map | `src/ui/world_map.c` |
| Monster Book, Forging Book, collection | `src/ui/books/` |
| Title, Game Start, Options, Name Entry, Password, sound test | `src/ui/title/` |
| Stage results, time result, challenges | `src/ui/results/` |
| Cable link, wireless adapter, link menus, link-play Django | `src/link/serial/`, `src/link/wireless/`, `src/link/{menu,panel,selection,lobby,records}/`, `src/link/player/` |
| Magic Cannon departure, bat flock, weather | `src/effects/screen/magic_cannon_departure.c`, `src/effects/bat_flock.c`, `src/effects/weather/` |
| Actor framework, collision, terrain, gameplay areas, camera | `src/scene/{actor,collision,terrain,areas,camera}/` |
| Tasks, heap, interrupts, boot; sprites, OAM, backgrounds | `src/engine/`, `src/engine/video/` |

## Finding a function's file

```sh
build/venv/bin/python tools/source_layout.py 08033568 Ghoul_Create
python3 tools/docindex.py 08033568
```

The address `08033568` resolves to `src/ui/dialogue/scrolling_dialogue.c`.
`Ghoul_Create` is a separate lookup by symbol.

## Where a new file goes

1. Decide what the code is in the game. A monster goes in `monsters/`, a thing
   placed in a map in `objects/`, a screen or overlay in `ui/`, a reusable
   visual in `effects/`, shared machinery in `scene/` or `engine/`.
2. If the object already has a directory, put it there. If the object has one
   or two files today, the new file joins them; at the third file make
   `src/<category>/<object>/` and move them in.
3. Name the directory after the object (the game's name if known), never after
   an address, registry key or slot number.
4. Put its header in `include/` at the mirrored path (at most two directories).
5. The author adds the placement rule in the same commit as the file. For C,
   record its path and ROM bounds in `symbols/source_files.csv` and use or add
   its owner in `symbols/source_dirs.csv`. For a header, record its owner path
   in `symbols/header_dirs.csv`. The test suite runs the same placement check
   as `tools/reorg.py --check`. `scope.py new FUNC` and `scope.py land FUNC`
   use these routes when they place drafted C.

## Layout tools

- `tools/source_layout.py FUNC` prints the file that defines a function.
  It accepts ROM addresses and local aliases as well as names from
  `symbols/functions.csv`.
- `tools/unit_files.py START END` lists all intersecting translation units
  in ROM order. Use `--strict` to flag a unit that extends beyond the range.
- `tools/check.py FUNC` checks the defining unit with the stock compiler.
- `tools/header_dependents.py HEADER --check` checks units that include a
  header directly or through another header.

`symbols/source_dirs.csv` and `symbols/header_dirs.csv` hold the reviewed
placement rules. `symbols/source_files.csv` records prior paths, ROM bounds,
and each source file's placement rule. `source_dirs.csv` assigns ROM ranges
for Player, Link Player, and intentional assembly. A recorded translation
unit stays intact at range boundaries. Progress report regrouping leaves
these tables alone. An uncovered ROM address or a legacy header with no
unique owner reports which table to update.

### Replay on a worker branch

1. Bring `tools/reorg.py`, `tools/source_layout.py`, and the three placement
   tables from `main` onto the worker branch.
2. Run `python3 tools/reorg.py` from the repository root. The preview should
   succeed, and every destination should use the current top-level layout.
3. Run `python3 tools/reorg.py --apply`, then `python3 tools/reorg.py --check`.
4. Commit the replay and merge `main` with a high rename limit, for example
   `git -c merge.renameLimit=20000 merge main`.

### Reviewed symbol and Player tables

Preview an exact symbol table with `tools/reorg.py --function-renames CSV`.
The table gives `addr`, `old_name`, and `new_name`. The tool checks the name
at each address, then rewrites all symbol references together. A prefix table
uses `--function-prefixes CSV` with `old_prefix`, `new_prefix`, and an
optional defining file or directory in `scope`. Use the exact table when
the reviewed scopes refer to a layout you have yet to apply.

`tools/reorg.py --player-moves CSV` previews a Player routing table with
`file`, `new_path` (a directory), and an optional `note`. It preserves each
source basename and records the previous path in the placement tables for
replay. Run the regular directory replay before applying a Player table to
an older worker branch.

Use `--apply` after reviewing a table's preview. These operations are separate
from the filename pass. Run the stock ROM build after any applied symbol table.
