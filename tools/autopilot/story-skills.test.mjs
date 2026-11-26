import assert from 'node:assert/strict';
import { landingReturn } from './higher-landings.mjs';
import fs from 'node:fs';
import test from 'node:test';
import { distanceFromEntrance, routeDistances, storyEntrances } from './route-entrances.mjs';

function skill(file, g, game = {}, exports = {}) {
  const source = fs.readFileSync(new URL(`lib/${file}.js`, import.meta.url), 'utf8');
  new Function('g', '$', 'exports', source)(g, game, exports);
  return exports;
}

test('conditional branches prefer known script flags with a physical-door fallback', () => {
  const banks = { 0x02000708: 0x02010000, 0x02000710: 0x02020000 };
  const g = {
    types: { globals: { gGameVariables: { addr: 0x02000708 }, gGameState: { addr: 0x02000710 } } },
    u32: (addr) => banks[addr],
    s16: (addr) => addr === 0x0201003c ? 42 : 0,
    u8: (addr) => addr === 0x0201006e ? 8 : 0,
  };
  const { doorBranches, conditionsAllow } = skill('conditions', g);
  const first = { box: 'CD24', scene: 3022, cond: ['not ((progress >= 40) && ({ proc_1200 } == 1))'] };
  const second = { box: 'CD24', scene: 3028, cond: ['((progress >= 40) && (r708[0x6e.2]:t4(4,1) == 1))'] };
  const absent = { box: 'CD23', scene: 3011, cond: ['(progress < 41)'] };
  assert.deepEqual(doorBranches([first, second, absent], (door) => door.box), [second, absent]);
  assert.equal(conditionsAllow(['(r708[0x3c]:t1 < 59)']), true);
  assert.equal(conditionsAllow(['(r708[0x3c]:t1 >= 59)']), false);
});

test('gun tactics use the equipped frame cost, including the free lens', () => {
  const player = { statModifierContext: { actionKind: 2 } };
  const equipped = { gun: true, lens: 0, energy: 2 };
  const g = { u8: (addr) => ({ 0x08e60f51: 5, 0x08e60f52: 3 })[addr] };
  const { gunFrame, gunTactics } = skill('gun', g, { gPlayers: [player] }, { weapon: () => equipped });
  assert.deepEqual(gunFrame(), { action: 2, power: 5, cost: 3 });
  assert.deepEqual(gunTactics('up'), []);
  equipped.lens = 1;
  assert.deepEqual(gunTactics('up')[1], [['up', 4], ['up+b', 14], [0, 15]]);
});

test('the best assigned gun frame uses Quick Change', () => {
  const base = 0x02010000;
  const bytes = [1, 10, 3, 1, 2, 15, 10, 1];
  const g = { types: { globals: { gGameState: { addr: 0x02000710 } } }, u32: () => base,
    u8: (address) => bytes[address - 0x08e60f4c],
    s16: (address) => ({ [base + 0x78]: 0, [base + 0x7a]: 1 })[address] ?? -1,
    u16: (address) => ({ [base + 0x4c0]: 1, [base + 0x4c2]: 2 })[address] ?? 0 };
  const lib = skill('gun', g, {}, {
    weapon: () => ({ gun: true, frame: 1, lens: 0 }), bags: () => ({ frames: [0, 1] }),
    quickChange: function* (want) {
      assert.deepEqual(want, { gun: true, frame: 0 });
      return { done: true };
    },
  });
  assert.deepEqual(lib.equipBestFrame().next(), { done: true,
    value: { kind: 'frame', id: 0, power: 10, cost: 3, done: true, method: 'quick change' } });
});

test('the free gun equips Luna before ranking the frame', () => {
  const selection = { id: 9, power: 50, cost: 0 };
  const lib = skill('gun', {}, {}, {
    bags: () => ({ lenses: [1, 2] }), session: { snapshot: () => ({}) },
    quickChange: function* (want) {
      assert.deepEqual(want, { gun: true, lens: 1 });
      return { done: true, frame: 1, lens: 1 };
    },
    weapon: () => ({ gun: true, lens: 1, frame: 9, energy: 2 }),
  });
  lib.equipBestFrame = function* () { return selection; };
  assert.deepEqual(lib.equipEnergyFreeGun().next(), { done: true, value: {
    done: true, gun: true, lens: 1, frame: 9, energy: 2, rankedFrame: selection,
  } });
});

test('gun inventory navigation wraps past empty cells and honors the vertical option menu', () => {
  const { gunGridRoute } = skill('gun', {});
  const cells = Array(36).fill(0xff);
  cells[0] = 0;
  cells[3] = 6;
  cells[19] = 19;
  cells[32] = 32;
  cells[34] = 39;
  assert.deepEqual(gunGridRoute(cells, 0, 0, 19), ['up', 'left']);
  assert.deepEqual(gunGridRoute(cells, 8, 2, 32, true), ['up']);
  assert.equal(gunGridRoute(cells, 8, 2, 19, true), null);
});

test('game over waits for the continue choice and selects its left entry', () => {
  const { gameOverMenu } = skill('recovery', {});
  assert.equal(gameOverMenu.active({ closeState: 0 }), false);
  assert.equal(gameOverMenu.ready({ closeState: 2 }), false);
  assert.equal(gameOverMenu.ready({ closeState: 3 }), true);
  assert.equal(gameOverMenu.decide({ choice: 1 }), 'left');
  assert.equal(gameOverMenu.decide({ choice: 0 }), 'a');
});

test('an exhausted terrain search rolls back a lethal final trial', () => {
  const player = { position: { x: 128, y: 512, z: 128 }, currentStatA: 240 };
  const lib = skill('world', { press: (key) => key }, { gPlayers: [player] });
  lib.where = () => ({ scene: 1, life: player.currentStatA });
  lib.session = {
    snapshot: () => [player.position.y, player.currentStatA],
    restore: (state) => { [player.position.y, player.currentStatA] = state; },
  };
  const search = lib.searchTo(1, 0);
  let result;
  while (!(result = search.next()).done) {
    player.position.y = 256;
    player.currentStatA = 0;
  }
  assert.deepEqual(result.value, { reached: false, reason: 'search failed' });
  assert.equal(player.position.y, 512);
  assert.equal(player.currentStatA, 240);
});

test('the fall guard accepts the surface of a downhill slope', () => {
  const player = { position: { x: 128, y: 512, z: 128 } };
  const lib = skill('world', {}, { gPlayers: [player] });
  const start = { flags: 0, kind: 0, level: 2 }, slope = { flags: 0, kind: 2, level: 2 };
  lib.grid = () => ({ at: (x) => x === 0 ? start : slope });
  lib.session = {
    snapshot: () => ({ ...player.position }),
    restore: (position) => { player.position = position; },
  };
  let height = 384, searched = 0;
  lib.stepTo = function* () {
    player.position = { x: 384, y: height, z: 128 };
    return { reached: true };
  };
  lib.searchTo = function* () { searched++; return { reached: false }; };
  assert.deepEqual(lib.safeStep(1, 0).next().value, { reached: true });
  assert.equal(searched, 0);
  player.position = { x: 128, y: 512, z: 128 };
  height = 250;
  assert.deepEqual(lib.safeStep(1, 0).next().value, { reached: false });
  assert.equal(searched, 1);
});

test('a solid detour takes precedence over exposed and passage-gated gaps', () => {
  const lib = skill('world', {});
  for (const hazard of [{ flags: 0x84a }, { flags: 0x82a, gatedGap: true }]) {
    const grid = { at: (x, z) => x >= 0 && x < 5 && z >= 0 && z < 3 ?
      { open: true, level: 3, ...(x === 2 && z === 1 ? hazard : { flags: 0 }) } : undefined };
    const route = lib.findPath(grid, [0, 1], new Set(['4,1']));
    assert.deepEqual(route.at(-1), [4, 1]);
    assert.ok(route.every(([x, z]) => x !== 2 || z !== 1));
  }
});

test('a moving floor links reachable solid landings across authored gap cells', () => {
  const lib = skill('moving-floors', {}, {}, skill('world', {}));
  const cells = Array.from({ length: 5 }, (_, x) => ({ x, z: 0, open: true, level: 3 }));
  const grid = { at: (x, z) => z === 0 ? cells[x] : undefined };
  const goals = new Set(['4,0']), gaps = new Set(['1,0', '2,0']);
  const board = { x: 384, z: 128 }, arrival = { x: 640, z: 128 }, floor = {};
  assert.equal(lib.findPath(grid, [0, 0], goals, gaps), null);
  assert.deepEqual(lib.planMovingFloor(grid, [0, 0], goals, gaps, [{ floor, points: [arrival, board], y: 768 }]),
    { floor, launch: cells[0], board, arrival, leave: cells[3], cost: 4 });
});

test('moving floor planning selects the first ride when the goal needs two', () => {
  const lib = skill('moving-floors', {}, {}, skill('world', {}));
  const cells = Array.from({ length: 8 }, (_, x) => ({ x, z: 0, open: true, level: 3 }));
  const grid = { at: (x, z) => z === 0 ? cells[x] : undefined };
  const point = (x) => ({ x: x * 256 + 128, z: 128 });
  const floors = [{ floor: 'left', points: [point(1), point(2)], y: 768 },
    { floor: 'right', points: [point(4), point(5)], y: 768 }];
  const goals = new Set(['7,0']), gaps = new Set(['1,0', '2,0', '4,0', '5,0']);
  assert.equal(lib.planMovingFloor(grid, [0, 0], goals, gaps, floors).floor, 'left');
  assert.equal(lib.planMovingFloor(grid, [3, 0], goals, gaps, floors).floor, 'right');
});

test('a lock tracks its saved bit across the script bank bytes', () => {
  let value = 0;
  const g = {
    types: { globals: { gGameVariables: { addr: 0x02000708 } } },
    u32: () => 0x02010000,
    u8: (addr) => addr === 0x02010063 ? value : 0,
  };
  const { closedLock } = skill('locks', g);
  const lock = { box: 'CD23', bit: 9, item: 44 };
  assert.equal(closedLock([lock], 'CD23'), lock);
  value = 2;
  assert.equal(closedLock([lock], 'CD23'), undefined);
});

test('a key targets the registered door object at unlock bit plus one', () => {
  const position = { x: 2048, y: 256, z: 512 };
  const slot = { id: 10, dir: 1, pos: { signedPosition: position } };
  const pool = { count: 1, slots: { $at: () => slot } };
  const g = {
    fn: { ResourceRegistry_GetResource: (id) => id === 0x85e6 ? 0x02010000 : 0 },
    at: () => pool,
  };
  const { keyDoorPosition } = skill('locks', g);
  assert.deepEqual(keyDoorPosition({ bit: 9 }), { ...position, direction: 1 });
  assert.equal(keyDoorPosition({ bit: 7 }), undefined);
});

test('doors to the same scene prefer the arrival nearer a forward exit', () => {
  const stage = { scene: 2, entrances: [{ id: 7, x: 0, z: 0 }, { id: 8, x: 0, z: 4096 }],
    doors: [{ scene: 1, x: 0, z: 64 }, { scene: 3, x: 0, z: 3840 }] };
  const distances = new Map([[1, 2], [2, 1], [3, 0]]);
  const distance = (entrance) => distanceFromEntrance({ scene: 2, entrance }, () => stage, distances);
  assert.equal(distance(7), 3840);
  assert.equal(distance(8), 256);
  assert.equal(distance(9), Infinity);
});

test('a failed high door can be retried from a higher entrance reached through another room', () => {
  const highDoor = { box: 'CD25', y: 1024, scene: 4 };
  const room = { scene: 1, doors: [{ box: 'CD22', y: 512 }, { box: 'CD24', y: 768 }, highDoor],
    entrances: [{ id: 7, box: 'CD22' }, { id: 9, box: 'CD24' }, { id: 10, box: 'CD25' }] };
  const stages = [room, { scene: 2, doors: [{ box: 'CD22', scene: 1, entrance: 7 }] },
    { scene: 3, doors: [{ box: 'CD23', scene: 1, entrance: 9 }] },
    { scene: 4, doors: [{ box: 'CD22', scene: 1, entrance: 10 }] }];
  assert.deepEqual(landingReturn(stages, room, highDoor, 512, (scene) => new Map([[1, scene]])),
    { room: 1, box: 'CD25', via: 3, returnBox: 'CD23', entrance: 9, height: 768, distance: 3 });
});

test('a disconnected low door can be approached from the lower landing', () => {
  const door = { box: 'CD23', y: 256, scene: 4 };
  const room = { scene: 1, doors: [{ box: 'CD22', y: 256 }, { box: 'CD24', y: 768 }, door],
    entrances: [{ id: 7, box: 'CD22' }, { id: 8, box: 'CD23' }, { id: 9, box: 'CD24' }] };
  const stages = [room, { scene: 2, doors: [{ box: 'CD23', scene: 1, entrance: 7 }] },
    { scene: 3, doors: [{ box: 'CD24', scene: 1, entrance: 9 }] }];
  assert.deepEqual(landingReturn(stages, room, door, 768, (scene) => new Map([[1, scene]])),
    { room: 1, box: 'CD23', via: 2, returnBox: 'CD23', entrance: 7, height: 256, distance: 2, descending: true });
});

test('goal routing avoids a failed shortcut and learns the longer path', () => {
  const stage = (scene, ...doors) => ({ scene, doors: doors.map(([box, to]) => ({ box, scene: to })) });
  const stages = [
    stage(1, ['A', 2], ['B', 3]), stage(2, ['C', 5], ['D', 1]),
    stage(3, ['E', 4]), stage(4, ['F', 5]),
  ];
  const failed = new Set();
  const pristine = routeDistances(stages, 5, failed);
  assert.equal(pristine.get(1), 2);
  failed.add('2:C');
  const reroute = routeDistances(stages, 5, failed);
  assert.equal(reroute.get(1), 3);
  assert.equal(reroute.get(2), 4);
  assert.equal(routeDistances(stages, 5, failed, '4:F').has(1), false);
});

test('a story goal routes through the active scripted entrance', () => {
  const target = { scene: 4, doors: [], entrances: [{ id: 7, cond: [] }, { id: 27, cond: ['(progress < 79)'] }] };
  const stages = [target, { scene: 1, doors: [{ box: 'A', scene: 4, entrance: 7 }, { box: 'B', scene: 2 }] },
    { scene: 2, doors: [{ box: 'C', scene: 4, entrance: 27 }] }];
  assert.equal(routeDistances(stages, 4, new Set()).get(1), 1);
  const allowed = storyEntrances(target, 79, () => true);
  assert.deepEqual([...allowed], [27]);
  assert.equal(routeDistances(stages, 4, new Set(), undefined, allowed).get(1), 2);
  assert.equal(storyEntrances(target, 79, () => false), undefined);
  target.doors.push({ box: 'D', scene: 2 });
  const transit = routeDistances(stages, 4, new Set(['1:B']), undefined, allowed);
  assert.equal(transit.get(4), 2);
  assert.equal(transit.get(1), 3);
});

test('live pressure plates use the center of the occupied square', () => {
  const manager = 0x02010000, slot = manager + 0x18;
  const words = { 0x020001a8: manager, [manager + 0x498]: 1,
    [slot + 0x48]: 0x5fc0, [slot + 0x4c]: 3145, [slot + 0x50]: 3146, [slot + 0x54]: 31 };
  const positions = { [slot + 0x1c]: 1536, [slot + 0x1e]: 256, [slot + 0x20]: 1536 };
  const g = { types: { globals: { gFloorSwitchManager: { addr: 0x020001a8 } } },
    u32: (address) => words[address] ?? 0, s16: (address) => positions[address] ?? 0, u16: () => 2 };
  const game = { gGameState: { region: 31 } };
  const { floorSwitches } = skill('floor-switches', g, game);
  assert.deepEqual(floorSwitches(), [{ addr: slot, name: 0x5fc0, x: 1664, y: 256, z: 1664, release: true, pressed: true }]);
  game.gGameState.region = 32;
  assert.deepEqual(floorSwitches(), []);
});

function switchProbe() {
  const plate = { addr: 0x02010018, x: 128, y: 256, z: 128, release: true };
  const cells = [{ x: 0, z: 0, open: true, level: 1 }, { x: 0, z: 1, open: true, level: 1 }];
  const task = { task: 4, update: 'DelayedScript_Update' };
  const state = { pressed: false, scheduled: false, restores: 0 };
  const game = {};
  const lib = skill('floor-switches', { press: (key) => key }, game, {
    where: () => ({ x: 128, y: 256, z: 384, scene: 2 }),
    grid: () => ({ at: (x, z) => x === 0 ? cells[z] : undefined }),
    cellOf: (x, z) => [x >> 8, z >> 8],
    findPath: () => [[0, 1]],
    navigate: function* (target) {
      assert.deepEqual(target, [128, 384]);
      return { reached: true };
    },
    tasks: () => state.scheduled ? [task] : [],
    session: { snapshot: () => null, restore: () => state.restores++ },
    DIR_KEYS: ['up+right', 'down+right', 'down+left', 'up+left'],
  });
  lib.floorSwitches = () => [{ ...plate, pressed: state.pressed }];
  return { trial: lib.probeFloorSwitch(plate, {}), state, game };
}

test('a delayed floor switch retreats on its first occupied tick', () => {
  const { trial, state } = switchProbe();
  assert.equal(trial.next().value, 'up+right');
  state.pressed = state.scheduled = true;
  assert.equal(trial.next().value, 'down+left');
  state.pressed = false;
  const result = trial.next();
  assert.equal(result.done, true);
  assert.equal(result.value.ready(), false);
  state.scheduled = false;
  assert.equal(result.value.ready(), true);
});

test('a pressure-plate preview rolls back a falling attack', () => {
  const { trial, state, game } = switchProbe();
  assert.equal(trial.next().value, 'up+right');
  state.pressed = true;
  game.gFallingObjectManager = { head: { $addr: 0x02014000, attack: 0x02002000, position: { x: 128, z: 128 } } };
  assert.equal(trial.next().value, 0);
  assert.deepEqual(trial.next(), { done: true, value: { hazard: true } });
  assert.equal(state.restores, 2);
});

test('the vampire model uses signed health and the source sunlight flag', () => {
  const addr = 0x02010000;
  const values = { [addr + 0x94]: 310, [addr + 0x9c]: 0x10, [addr + 0x3ac]: 1, [addr + 0x54]: 400 };
  const lists = Array.from({ length: 9 }, () => ({ head: null }));
  lists[8].head = { $addr: addr, flags: 0, update: 0x08028f25, next: null };
  const g = { addr: () => 0x08028f25, u32: (a) => values[a] ?? 0, s16: (a) => values[a] ?? 0, u8: (a) => values[a] ?? 0 };
  const { vampireTargets, combatWait } = skill('combatants', g, { gTaskLists: lists });
  const [vampire] = vampireTargets();
  assert.equal(vampire.health, 310);
  assert.equal(vampire.x, 400);
  assert.equal(vampire.solarWeak, true);
  assert.equal(combatWait(vampire), 24);
  values[addr + 0x384] = 1;
  assert.equal(vampireTargets()[0].solarWeak, false);
  assert.equal(vampireTargets()[0].solarReactive, true);
  values[addr + 0x9c] |= 2;
  assert.deepEqual(vampireTargets(), []);
});

test('Hresvelgr is tracked by the live boss pointer and its own contact state', () => {
  const addr = 0x02010000;
  const actor = { health: 730, flags: 0, position: { x: 640, y: 512, z: 1920 } };
  const g = { types: { globals: { gHresvelgr: { addr: 0x02000008 } } },
    u32: () => addr, u16: (offset) => offset === addr + 0x318 ? 17 : 0,
    at: () => actor, fn: { AreaEffect_IsPositionCovered: (position) => Number(position === addr + 0x54) } };
  const { hresvelgrTargets } = skill('hresvelgr', g);
  assert.deepEqual(hresvelgrTargets(), [{ addr, boss: 'Hresvelgr', health: 730, alive: true,
    x: 640, y: 512, z: 1920, hitLock: 17, solarReactive: true }]);
  actor.flags = 2;
  assert.deepEqual(hresvelgrTargets(), []);
});

test('Solar Trance activates ability one and waits for form two', () => {
  const player = { $addr: 8, statHundredths: 1000, actionMode: 0, tetherMode: 0 };
  const g = { fn: { Player_GetSelectedAbility: () => 1 }, press: (key) => key,
    until: (ready) => ready };
  const lib = skill('trance', g, { gPlayers: [player] }, { session: { snapshot: () => ({}) } });
  assert.equal(lib.solarTranceReady(), true);
  const entry = lib.enterSolarTrance();
  assert.equal(entry.next().value, 'l');
  const ready = entry.next().value;
  assert.equal(ready(), false);
  player.actionMode = 2;
  assert.equal(ready(), true);
  assert.deepEqual(entry.next(), { done: true, value: true });
});

test('Solar Bomb and movement trials look beyond the fuse at the same frame', () => {
  const { solarBombLook, solarTranceTactics } = skill('trance', {});
  const tactics = [[['up', 12]], ...solarTranceTactics('up')];
  const endpoints = tactics.map((tactic) => {
    const elapsed = tactic.reduce((frames, [, hold]) => frames + hold, 0);
    return elapsed + solarBombLook(elapsed);
  });
  assert.deepEqual(endpoints, [180, 180, 180]);
  assert.deepEqual(solarTranceTactics()[0], [['a', 2], [0, 20]]);
});

test('combat waits while a story script controls the player', () => {
  let scripted = true, scene = 1;
  const g = { frameNo: 1, fn: { ScriptUtility_IsControlFlagSet: () => scripted },
    until: (ready, timeout) => ({ ready, timeout }) };
  const lib = skill('world', g, { gPlayers: [{ currentStatA: 70 }] });
  Object.assign(lib, {
    session: { solar: 232, setSolar: () => {} },
    where: () => ({ scene }), message: () => ({ open: false }),
    weapon: () => ({ gun: false }), fightTargets: () => [{ life: 10 }],
    clearInterruptions: function* () { return false; },
  });
  const fight = lib.fight();
  const wait = fight.next().value;
  assert.equal(wait.ready(), false);
  assert.equal(wait.timeout, 120);
  scripted = false;
  assert.equal(wait.ready(), true);
  scene = 2;
  const result = fight.next();
  assert.equal(result.done, true);
  assert.equal(result.value.steps, 0);
});

test('coffin navigation holds A until the exit text opens', () => {
  const player = { $addr: 0x02010000, tetherMode: 0 };
  const g = { keys: 0, keyMask: () => 1, fn: {
    Player_TestNearbyInteraction: () => true, ScriptUtility_IsControlFlagSet: () => false,
  },
    press: (key, frames) => ({ key, frames }) };
  const game = { gPlayers: [player], gTetherActor: { interactionScriptId: 841, position: { x: 128, z: 128 } } };
  const lib = skill('coffin', g, game, {
    session: { snapshot: () => null, restore: () => {} },
    navigate: function* (target, opts) {
      assert.equal(target, 'CD22');
      assert.equal(g.keys, 1);
      assert.equal(opts.stopIf(), false);
      player.tetherMode = 1;
      assert.equal(opts.stopIf(), true);
      opts.beforeText();
      assert.equal(g.keys, 0);
      assert.ok(!opts.stopIf());
      return { reached: true };
    },
  });
  assert.equal(lib.coffinAvailable(), true);
  const carry = lib.carryCoffin('CD22', {});
  assert.deepEqual(carry.next().value, { key: 'a', frames: 2 });
  player.tetherMode = 6;
  assert.deepEqual(carry.next(), { done: true, value: { grabbed: true, reached: true } });
  assert.equal(g.keys, 0);
});

test('purification brings the trailing coffin across the live panel center', () => {
  let story = 59;
  const panel = { initialMode: 0, effectState: 0, subjectCount: 0, subjects: [], center: { x: 100, z: 100 } };
  const game = { gScenePanel: panel, gTetherActor: { position: { x: 100, z: 0 } },
    gPlayers: [{ position: { x: 100, z: 90 } }] };
  const lib = skill('purification', { fn: { ScriptUtility_IsControlFlagSet: () => false } }, game, {
    where: () => ({ scene: 1 }), story: () => story, coffinAvailable: () => true,
    carryCoffin: function* (target, opts) {
      assert.deepEqual(target, [100, 190]);
      assert.equal(opts.stopIf(), false);
      panel.effectState = 1;
      assert.equal(opts.stopIf(), true);
      story++;
      return { grabbed: true, reached: false, reason: 'stopped' };
    },
  });
  assert.equal(lib.purificationPanel(), true);
  assert.deepEqual(lib.preparePurification().next(), { done: true, value: { done: true } });
});

test('Battle Drive waits while the rig is trapped above the coffin', () => {
  let scene = 1, threshold;
  const g = { frameNo: 0, fn: { ScriptUtility_IsControlFlagSet: () => false },
    press: (key, frames) => ({ key, frames }) };
  const game = { gScenePanel: { initialMode: 1, effectState: 10, rig: { state: 2 } },
    gGameState: { unk618: 100 }, gPlayers: [{ currentStatA: 80 }] };
  const lib = skill('purification-battle', g, game, {
    session: { setSolar: (value) => { threshold = value; } },
    where: () => ({ scene }), story: () => 59,
    clearInterruptions: function* () { return false; },
  });
  assert.equal(lib.purificationBattle(), true);
  const fight = lib.fightPurification();
  assert.deepEqual(fight.next().value, { key: 0, frames: 30 });
  assert.equal(threshold, 64);
  game.gGameState.unk618 = 0;
  scene = 2;
  assert.deepEqual(fight.next(), { done: true, value: { done: true, steps: 0, darkness: 0, life: 80 } });
});

test('bike search keeps steering that avoids damage on the same stretch', () => {
  const motion = { position: { x: 0, z: 0 }, style: { initialWord: 80 } };
  const g = { name: () => 'Glove_UpdateStopping', press: (key, frames) => ({ key, frames }) };
  const lib = skill('bike', g, {}, {
    session: {
      snapshot: () => structuredClone(motion),
      restore: (state) => { Object.assign(motion.position, state.position); Object.assign(motion.style, state.style); },
    },
    where: () => ({ scene: 1 }), story: () => 64, message: () => ({ open: false }),
  });
  const ride = lib.rideBike({ motion }, { horizon: 32 });
  let step = ride.next();
  for (; !step.done; step = ride.next()) {
    const { key, frames } = step.value;
    if (key === 'r+up') motion.position.z = -12 * 4096;
    if (key !== 0) motion.position.x += frames * 4096;
    if (motion.position.x === 32 * 4096 && motion.position.z === 0) motion.style.initialWord -= 20;
  }
  assert.deepEqual(step.value, { key: 'r+up', frames: 24, x: 32, lane: -12, life: 80 });
});

test('a lift door first approaches a reachable landing at the platform height', () => {
  const player = { position: { x: 128, y: 256, z: 640 } };
  const lift = { kind: 2, state: 4, name: 7, lowerHeight: 512, upperHeight: 1280,
    body: { position: { x: 384, y: 512, z: 384 } } };
  const box = { name: 'CD42', x0: 256, x1: 512, z0: 256, z1: 512 };
  const g = { types: { globals: { gNamedActorManager: { addr: 1 } } }, u32: () => 2,
    at: () => ({ used: 1, slots: [lift] }), press: (key, frames) => ({ key, frames }) };
  const lib = skill('lifts', g, { gPlayers: [player] }, {
    boxes: () => [box], cellOf: (x, z) => [x >> 8, z >> 8], coffinAvailable: () => false,
    grid: () => ({ at: (x, z) => ({ x, z, level: x === 1 && z === 2 ? 2 : 1, open: true }) }),
    findPath: (grid, here, goals) => {
      assert.deepEqual([...goals], ['1,2']);
      return [here, [1, 2]];
    },
    navigate: function* (position) {
      assert.deepEqual(position, [384, 640]);
      player.position.y = 512;
      return { reached: true };
    },
  });
  assert.equal(lib.liftAt(box), lift);
  const approach = lib.approachLift('CD42');
  assert.deepEqual(approach.next().value, { key: 0, frames: 1 });
  assert.deepEqual(approach.next(), { done: true, value: { name: 7, height: 512, target: [384, 640], reached: true } });
});

test('simultaneous plates require a block on each goal cell', () => {
  const w = 5, h = 3, blocks = [{ x: 1, z: 1, kind: 0 }, { x: 3, z: 1, kind: 0 }];
  const cells = Array.from({ length: w * h }, (_, i) => ({
    level: blocks.some((b) => i === b.z * w + b.x) ? 2 : 1, slope: false, open: true, flags: 0,
  }));
  const { planPushes } = skill('world', {});
  const goals = new Set(['0,1', '4,1']);
  assert.deepEqual(planPushes({ w, h, cells }, [2, 2], goals, blocks), []);
  const plan = planPushes({ w, h, cells }, [2, 2], goals, blocks, 100, { occupy: true });
  assert.deepEqual(plan.map((move) => [move.x, move.z, move.dir]), [[1, 1, 3], [3, 1, 1]]);
});

test('a block that survives a drop occupies the lower pressure plate', () => {
  const cells = [1, 3, 2].map((level) => ({ level, slope: false, open: true, flags: 0 }));
  const { planPushes } = skill('world', {});
  const result = planPushes({ w: 3, h: 1, cells }, [2, 0], new Set(['0,0']),
    [{ x: 1, z: 0, kind: 0, pushes: Infinity, survivesFall: true }], 100, { occupy: true });
  assert.deepEqual(result, [{ x: 1, z: 0, dir: 3, from: [2, 0] }]);
});

test('armor targets model damage to the guard and the contact cooldown', () => {
  const fields = { 0xb1c: 240, 0xb20: 65, 0xb2c: 9, 0xb32: 2 };
  const read = (addr) => fields[addr - 0x02010000] ?? 0;
  const { armorCombatant } = skill('armor', { u16: read, s16: read });
  assert.deepEqual(armorCombatant(0x02010000), { boss: 'Armor', health: 240, alive: true, hitLock: 9, guard: 65 });
  fields[0xb32] = 0x80;
  assert.equal(armorCombatant(0x02010000).alive, false);
  assert.equal(armorCombatant(0x02010000).guard, 0);
});

test('dark Trance requires a full gauge and uses the transformed actions', () => {
  const player = { actionMode: 0, tetherMode: 1, statHundredths: 999 };
  const { darkTranceReady, darkTranceTactics } = skill('trance', {}, { gPlayers: [player] });
  assert.equal(darkTranceReady(), false);
  player.statHundredths = 1000;
  assert.equal(darkTranceReady(), true);
  player.tetherMode = 3;
  assert.equal(darkTranceReady(), false);
  assert.deepEqual(darkTranceTactics('up')[0], [['up', 4], ['a', 2], [0, 18]]);
});

test('curtain segments are targets only while standing and have hit cooldowns', () => {
  const lists = Array.from({ length: 9 }, () => ({ head: null }));
  lists[8].head = { $addr: 0x02010000, flags: 0, update: 0x0801324d, next: null };
  const position = { x: 300, y: 512, z: 500 };
  const barrier = { $addr: 0x02010050, state: 1, health: 2, flash: 6, sprite: { position } };
  const g = { addr: () => 0x0801324d, at: () => ({ count: 2, barriers: [barrier, { state: 2 }] }) };
  const { barrierTargets, combatWait } = skill('combatants', g, { gTaskLists: lists });
  const [target] = barrierTargets();
  assert.equal(barrierTargets().length, 1);
  assert.equal(target.health, 2);
  assert.equal(target.y, 512);
  assert.equal(combatWait(target), 6);
});
