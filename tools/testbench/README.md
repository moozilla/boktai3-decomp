# Game testbench

Tags: #tooling #actors #scripts #coverage

Run Boktai 3 scenarios against the source port. Use the game's registry to
create actors, read their typed fields, and record which functions ran. A case
starts from a private battery fixture in a fresh game subprocess. The parent
reports frame-budget failures, wall timeouts, wasm traps, process crashes and
assertion failures separately.

## Run a case

From the repository root, build the port and its typed views once.

```sh
build/venv/bin/python port/build_game.py -j2
build/venv/bin/python port/agent/gametypes.py -j2
make -f tools/emu/Makefile build/studio/experiments/harness
node tools/testbench/run.mjs tools/testbench/examples/ghoul.mjs
node tools/testbench/run.mjs tools/testbench/examples/menu.mjs --case 'pause/sword'
node tools/testbench/run.mjs --catalog 'Wooden Crate'
node --test tools/testbench/test.mjs
```

The harness target above builds a private copy with the current tick commands.
Set `BOKTAI3_HARNESS` to use another compatible build. Use `--no-rom` to run
just the port. Results, input movies, screenshots and ROM harness logs go under
`build/testbench/`. `--out`, `--frames` and `--timeout` set the output
directory, frame budget and wall deadline in seconds.

## Write a scenario

```js
import { scenario } from '../scenario.mjs';

export default scenario({ empty: true, sun: 6, time: '14:00',
  equip: { sword: 'Gradius', lens: 'Flame' }, items: { 'Solar Fruit': 3 },
}, function* (s) {
  const { x, y, z } = s.player.position;
  const ghoul = s.spawn('Ghoul', { x: x + 96, y, z });
  s.expect(ghoul.hp).equals(ghoul.maxHp);
  yield s.press('b', 4);
  yield s.run(60);
  s.damage(ghoul, 9);
  s.expect(ghoul.hp).below(ghoul.maxHp);
  yield* s.screenshot('ghoul-hit');
});
```

The generator runs in a frame callback. `yield` advances the game and
`yield*` runs a helper that may advance several frames. Calls, spawn, checks
and typed reads are synchronous. Direct C calls use the port's word-based
call adapters. Functions with aggregate signatures need a matching adapter
before they can be called by name.

| Interface | Purpose |
| --- | --- |
| `scenario({ save, scene, entrance, story, flags }, body)` | Use a battery save or checkpoint `.sgm` and write the requested scene, entrance and saved flags into a checksummed copy. The game still runs that scene's startup script. |
| `empty: true` | Use Sealed Prison scene 2776, entrance 7, story 25. The fixture starts with no ambient registry enemies. It retains native geometry, exits and the field managers. |
| `stage: 'warp'` | Continue the base save, set persistent variables, call the game's door transition, then apply local overrides. |
| `field: false`, `coverageFrom` | Permit a scene with no player, such as a standalone panel, and optionally start coverage at a named function during scene setup. |
| `time`, `solar`, `sun` | Fixed RTC (UTC), raw cartridge sensor (0 bright through 255 dark), or forced gauge level (0–10). `s.setSolar(raw)` records sensor changes in the replay. |
| `equip`, `items` | Assign a sword and lens by name or ID, and add inventory items through the native inventory API. The lens is selected for the gun even when the sword is in hand. |
| `s.spawn(name, { x, y, z, options, args, recipe, id })` | Create by game name, registry key or callback. `options` uses exact GCL keyword letters. Set a keyword to `null` to omit it. Supply `args` to override the positional list after the registry key, or `id` to change the first `chara` argument. |
| `actor.view`, `actor.base`, `actor.hp`, `actor.position` | Typed views of the concrete object and its shared base when identified. If a creator allocates several things, `actor.created` lists the observed allocations. |
| `s.place(actor, position)`, `s.state(actor, fields)`, `s.damage(actor, amount)` | Move, write typed fields, or exercise the forced damage path. The normal attacks are available via input. |
| `s.variable(key, index)`, `s.setVariable(key, value, index)` | Read or write a GCL storage descriptor such as `r708:0x3c:t1` or `r700:0x6e.1:t4`. |
| `s.callproc(id, ...args)`, `s.call(symbol, ...words)`, `s.c(source, headers)` | Run a GCL procedure, call a compiled game function, or compile a C snippet with the game's prepared headers. |
| `s.command(name, options, args)`, `yield* s.atTick(() => action())` | Run a registry command on existing game state, and schedule synchronous work at the next scheduler entry. Use the scheduler entry for creators that read EEPROM or initialize serial hardware. |
| `s.$`, `s.typed(address, type)`, `s.allocate(type, fields)` | Access typed globals, inspect a known address and supply temporary typed argument objects. |
| `yield* openMenu(s, name)` | Open a named pause page or instantiate a native shop, book or link display. Pause pages use controller input. The link displays use a synthetic local session. |
| `s.expect(value, label)`, `yield* s.waitFor(test, frames, label)`, `yield* s.screenshot(name)` | Assertions, bounded waits and rendered 240 × 160 PNGs. |

See [ghoul](examples/ghoul.mjs), [props and projectiles](examples/props-and-projectiles.mjs),
[procedure and C](examples/script-and-c.mjs), [menus](examples/menu.mjs) and
[ROM replay](examples/replay.mjs) and [Solar Bank](examples/solar-bank.mjs)
for complete cases. The Ghoul example checks both a normal sword hit and a
normal hit back before forcing damage.

## Catalog and sweep

`catalog.json` indexes the tracked registry keys, the observed procedure and
scene associations, the Studio's parameter schema, and a small set of
observed argument recipes per key. A recipe's `unresolved` fields depend on
the original procedure's arguments, expressions or locals. Supply those
fields explicitly when the fixture requires them. Reviewed manager and scene
requirements come from `contexts.json`. Registry commands that
only operate on an existing object are indexed for lookup. The sweep selects
the candidate creators and the native pool entries with authored fixtures.
GCL names are used where recovered. Key and function aliases cover unnamed
entries and disambiguate duplicate names.

```sh
build/venv/bin/python tools/testbench/catalog.py --check
node tools/testbench/sweep.mjs
node tools/testbench/sweep.mjs --filter 'Ghoul|pause/sword|Projectile' --no-rom
```

The actor smoke case tries creation near the player, 600 idle frames, an input
attack and forced defeat for discovered live health-bearing objects. The result
records which actors stayed live, whether health moved after input and whether
the player took damage. One-shot effects and many menus expire on their own.
Some creators require their original parent, resource setup or a repeatable
scene-specific recipe. The current catalog covers the registry and native
scene particle and projectile fixtures. Other C-only creators still
need individual recipes.

Coverage starts after staging. `coverage.json` lists every function in the
checked-in progress inventory with the scenarios that entered it.
`unreached.md` groups unexecuted functions by unit. `sweep.jsonl` and
`summary.md` keep the per-case results and failure details. The verified
sweep reached **38.39%** (4,534 of 11,810) across 206 passing cases, a gain
of 2.91 percentage points from the first diagnostic run. All 11 input-only
cases matched the ROM on the sampled state. The remaining cases recorded
their port-only setup operations.

## Original ROM comparison

The runner replays the generated battery, RTC, raw sensor and input movie
through mGBA. Inputs align at each call of `Input_UpdatePadStates` so lag
frames do not shift a case. It compares scene, story, saved life and player
position at the last recorded input tick. Results say `unreproducible` for
the fixture's port-only operations and list the ones that were used.
Direct manipulation through `s.host` bypasses tracking. Set `replay: false`
if a case adds such operations or mutates game state through raw agent calls.

Use `pending_src_fixes/` for evidence-backed source patches found here
while another batch owns the game source.
