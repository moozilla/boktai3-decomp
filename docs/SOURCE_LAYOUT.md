# Source layout

Tags: #tooling #process

The C source is arranged by what each piece of code *is in the game*, the way
zeldaret's tmc and pret's pokeemerald arrange theirs. To find something, go to
the category you would guess as a player, then to the object by its game name.
Every `.c` file is matched C. Link order comes from function addresses
(`tools/split.py`), not from directories, so a file's place in the tree is free
to follow meaning.

## The map

| Directory | What is in it |
|---|---|
| `src/player/` | Django: core, movement, melee attacks, the gun, taking damage, stats, object interactions, animation cues, cutscene commands, appearance, special actions, and link actions |
| `src/monsters/` | Every enemy family, one file or directory per monster (the game's own word: the Monster Book, 魔物図鑑) |
| `src/bosses/` | The Immortals (Hresvelgr, Nidhoggr, Ratatosk, Vanargand, Sabata) and the Vampire |
| `src/npc/` | Townspeople, story characters and cutscene actors, including Sabata's bike rider |
| `src/objects/` | Coffin, crates, switches, Warp Circle, Solar Stand, Pile Driver, ship deck cannons, Solar Roots, sun orbs, items, hazards, and markers |
| `src/effects/` | Palette, particles, screen effects, projectiles, bursts, orbits, sprites, bat flocks, shadows, and weather |
| `src/ui/` | HUD, dialogue, pause menu, town and world maps, town services, books, title screens, and results |
| `src/minigames/` | Django's Coffin Bike, the Dark Generator, and the result (transformation) minigame |
| `src/link/` | Two-player play: cable link (`serial`), wireless (`wireless/`), the link menus and the other player's avatar |
| `src/system/` | The cartridge and persistent state: solar sensor, clock and sun schedule, saving, game state, inventory data |
| `src/scene/` | The world simulation shared by everything that moves: the actor framework, collision, terrain and paths, camera, scene lifecycle |
| `src/script/` | The GCL script interpreter (Metal Gear Solid's scripting language) and its runtime |
| `src/engine/` | Boot, tasks, heap, interrupts, video (backgrounds, sprites, palettes, animation, text tiles), math, game-side sound |
| `src/lib/` | Third-party libraries kept in their upstream form: m4a (sound driver), rfu (wireless), rtc, eeprom, libgcc, libc, libm |

## Conventions

* `src/<category>/<object>/` for an object with three or more files;
  `src/<category>/<object>.c` for a one-file object; a two-file object's files
  sit directly in the category (`monsters/ghoul.c`, `monsters/ghoul_states.c`).
  Four two-file objects have a directory because their file names would not say
  what they belong to: `objects/coffin/`, `objects/bat_curtain/`, `ui/save/`,
  `ui/game_over/`.
* A middle `<group>/` level only where a category has natural families:
  `effects/{palette,particles,screen,projectiles,bursts,orbits,sprites,scene}/`,
  `ui/{hud,dialogue,pause,town,books,title,results,widgets}/`, `link/wireless/`,
  `monsters/cannons/`, `objects/{hazards,items,markers}/`,
  `scene/{actor,collision,terrain}/`, `player/effects/`, `engine/{video,math,sound}/`.
  Never more than three directory levels below `src/`.
* Directory names are short lowercase snake_case nouns, the game's own name for
  the thing where it has one (from `gcl/names/chara.csv`, the monster list in
  the ROM text, and the function prefixes in `symbols/functions.csv`). No
  numbers, no hex, no "tentative": an unidentified object gets the most honest
  description of what it does, and [the layout decisions](SOURCE_LAYOUT_DECISIONS.md) says it is a guess.
* Each object is one directory: code that belongs to one object (helpers,
  projectiles, sub-parts) lives with it, even when the report split it.
* Headers sit beside their object under the mirrored path in `include/`, at most
  two directories deep: `#include "monsters/ghoul.h"`,
  `#include "bosses/vanargand/vanargand_projectiles.h"`,
  `#include "ui/pause/pause_menu.h"`. A three-level source directory such as
  `src/ui/pause/map/` keeps its headers in `include/ui/pause/`. `global.h` and
  `debug.h` are at the root of `include/`. Types genuinely shared across
  categories live with the subsystem that defines them: `include/engine/`
  (memory, tasks, input, RAM layout), `include/engine/video/` (display
  sequences, palettes, render), `include/engine/math/` (vectors, interpolation,
  random helpers), `include/scene/actor/` (the actor framework every monster
  uses). `include/gba/` holds the SDK headers.
* A one-function file uses the exact symbol spelling (`Hresvelgr_Create.c`).
  An unnamed function keeps its address (`sub_08033568.c`). A multi-function
  translation unit is named for what it contains. `lib/` keeps upstream file
  names. The directory pass preserves existing basenames except where the
  reviewed table explicitly gives a new one.

## Where is it?

| You are looking for | Go to |
|---|---|
| Django (the player object: create, update, draw, modes) | `src/player/core/` |
| Walking, turning, slopes, pushing forces, the camera following Django | `src/player/movement/` |
| Melee weapons: slash, spin, sweep, throw, combos, attack power | `src/player/attack/` |
| The solar gun: charged shots, projectiles, charge tiers | `src/player/gun/` (identification of gun vs. melee is partly by name) |
| Getting hit, knockback, defeat and death | `src/player/damage/` |
| Life, energy, resistance, equipment values | `src/player/stats/` |
| Dragging the coffin, pushing blocks, using a Solar Stand | `src/player/interaction/` |
| Django's animation cues and form changes | `src/player/cues/` |
| Cutscene commands that move Django (kneel, aim, shoot) | `src/player/script/` |
| Django's sprite model, palettes, visuals | `src/player/appearance/` |
| Django's barrier, attack and burst effects | `src/player/effects/` |
| Special motion effects, including Spiral Burst | `src/player/special_actions/` |
| Link Player dispatchers | `src/player/link_actions/` |
| Lock-on targets | `src/player/target_registry/` |
| Ghoul (ボク family) | `src/monsters/ghoul.c`, `ghoul_states.c` |
| Bat, Bee, Slime, Ghost, Crow, Spider, Mimic, Octopus, Dog (Hound, Hot Dog, Ice Dog), Lun | `src/monsters/<name>.c` |
| Skeleton, Golem, Mummy, Vorn | `src/monsters/<name>/` |
| Armor and the elemental armors | `src/monsters/armor/` |
| Statue | `src/monsters/statue/` |
| Sword, Axe | `src/monsters/sword/`, `src/monsters/axe/` |
| Ivy, Centipede, Popbud | `src/monsters/ivy/`, `centipede/`, `popbud/` |
| Ooze, Screamer, Poison Hand, Dragon Puppy | `src/monsters/<name>/` |
| Poisoner and Perplexer | `src/monsters/pollen_flowers/` |
| Garmr Statue and Muspell Head | `src/monsters/garmr_statue/`, `muspell_head/` |
| Vulcan, Wind, and Plasma Cannons | `src/monsters/cannons/{vulcan,wind,plasma}/` |
| Hresvelgr (eagle) | `src/bosses/hresvelgr/` |
| Nidhoggr (dragon), including its grid attacks | `src/bosses/nidhoggr/` |
| Ratatosk | `src/bosses/ratatosk/` |
| Vanargand (wolf), its projectiles, parts and bike-chase appearance | `src/bosses/vanargand/` |
| Sabata | `src/bosses/sabata/` |
| Sabata riding the purple bike | `src/npc/sabata_rider/` |
| The Vampire and its bat swarm | `src/bosses/vampire/` |
| Townspeople and NPCs | `src/npc/scripted_npc/`, `src/npc/tagged_npc/` |
| Story characters in cutscenes | `src/npc/story_character/`, `src/npc/cutscene_actor/` |
| The coffin | `src/objects/coffin/` |
| Warp Circle | `src/objects/warp_circle/` |
| Solar Stand | `src/objects/solar_stand/` |
| Pile Driver (purification) | `src/objects/pile_driver/` |
| Ship deck cannons and Solar Roots | `src/objects/deck_cannons/`, `solar_roots/` |
| Wooden crate, floor switch, hit switch, Bat Curtain | `src/objects/wooden_crate/`, `floor_switch/`, `hit_switch.c`, `bat_curtain/` |
| Treasure chests and dropped items | `src/objects/items/` |
| Sun orbs, the solar bomb | `src/objects/sun_orb/`, `src/objects/solar_bomb/` |
| Traps and hazards | `src/objects/hazards/` |
| The solar sensor (GPIO), solar level, calibration screen | `src/system/solar/` |
| The clock, sunrise and sunset, region setting | `src/system/clock/` (RTC chip driver: `src/lib/rtc/`) |
| Saving and loading | `src/system/save/` (EEPROM driver: `src/lib/eeprom/`), save screen `src/ui/save/` |
| Story flags, game state, challenge records, Solar Fruit | `src/system/game_state/` |
| Items and equipment data | `src/system/inventory/` |
| Music and sound effects | `src/engine/sound/` (game side), `src/lib/m4a/` (driver) |
| The script interpreter (GCL) | `src/script/script_virtual_machine.c`; runtime `src/script/runtime/`; cutscene runner `src/script/demo_manager.c`; the scripts themselves: `gcl/` and the `gcx` resource |
| Script commands of an object (`*_FromScript`) | in that object's own directory |
| Dialogue boxes, text commands, text windows | `src/ui/dialogue/` |
| HUD (life, energy, sun gauge), damage numbers | `src/ui/hud/display/`, `src/ui/hud/damage_numbers/` |
| Pause menu, its inventory, map, status panel | `src/ui/pause/` |
| Pause Status options and menu sequences | `src/ui/pause/status_options.c`, `menu_sequences/` |
| Status screen | `src/ui/status_screen/` |
| Weapon shop | `src/ui/town/shop/` |
| San Miguel town map and world destination map | `src/ui/town/town_map.c`, `src/ui/world_map.c` |
| Solar Bank, Dark Loan, Storage | `src/ui/town/solar_bank/`, `dark_loan.c`, `storage/` |
| Monster Book, Forging Book, collection | `src/ui/books/` |
| Equipment modifier menu | `src/ui/modifier_menu/` |
| Title, Game Start, Options, Name Entry, Password, sound test | `src/ui/title/` |
| Stage results, time result, challenges | `src/ui/results/` |
| Game over | `src/ui/game_over/` |
| Django's Coffin Bike and the Dark Generator | `src/minigames/coffin_bike/`, `dark_generator.c` |
| Cable link | `src/link/serial/`, `handshake/`, `session/` |
| Wireless adapter and connection | `src/link/wireless/connection/`, `rfu_manager/`; SDK `src/lib/rfu/` |
| Wireless battle, trading | `src/link/wireless/battle/`, `trade_menu/` |
| Link lobby, records, scores | `src/link/lobby/`, `records/`, `score_panel/` |
| Palette fades and cycling | `src/effects/palette/` |
| Particles | `src/effects/particles/` |
| Screen effects (scanline, flashes, blends, scrolling backdrops) | `src/effects/screen/` |
| Magic Cannon departure and the cutscene bat flock | `src/effects/screen/magic_cannon_departure.c`, `src/effects/scene/bat_flock.c` |
| Projectiles | `src/effects/projectiles/` |
| Explosions and bursts, healing sparkles | `src/effects/bursts/`, `src/effects/healing/` |
| Weather and storms, shadows | `src/effects/weather/`, `src/effects/shadows/` |
| Actor framework shared by monsters (behavior, reactions, placement) | `src/scene/actor/` |
| Collision | `src/scene/collision/` |
| Terrain, paths, navigation mesh | `src/scene/terrain/` |
| Camera | `src/scene/camera/` |
| Tasks, heap, interrupts, boot (`AgbMain`) | `src/engine/` |
| Sprites, OAM, backgrounds, palettes, text tiles | `src/engine/video/` |

## Finding a function's file

```sh
build/venv/bin/python tools/source_layout.py 08033568 Ghoul_Create
python3 tools/docindex.py 08033568
```

The resolver reads definitions, placeholders, aliases and the symbol table, so
it answers for any address or name. `symbols/functions.csv` gives the name; the
file is the function's own `.c` or the unit that contains it. The address
`08033568` resolves to `src/ui/dialogue/scrolling_dialogue.c`. `Ghoul_Create`
is a separate lookup by symbol.

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
and per-file Player placement. `source_dirs.csv` also assigns the ROM ranges
that contain intentional assembly. Progress report regrouping leaves these
tables alone. An uncovered ROM address or a legacy header with no unique owner
reports which table to update.

### Replay on a worker branch

1. Bring `tools/reorg.py`, `tools/source_layout.py`, and the three placement
   tables from `main` onto the worker branch.
2. Run `python3 tools/reorg.py` from the repository root. The preview should
   succeed, and every destination should use the current top-level layout.
3. Run `python3 tools/reorg.py --apply`, then `python3 tools/reorg.py --check`.
4. Commit the replay and merge `main` with a high rename limit, for example
   `git -c merge.renameLimit=20000 merge main`.
