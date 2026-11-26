import assert from 'node:assert/strict';
import fs from 'node:fs';
import test from 'node:test';
import { distanceFromEntrance } from './route-entrances.mjs';

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
  const { doorBranches } = skill('conditions', g);
  const first = { box: 'CD24', scene: 3022, cond: ['not ((progress >= 40) && ({ proc_1200 } == 1))'] };
  const second = { box: 'CD24', scene: 3028, cond: ['((progress >= 40) && (r708[0x6e.2]:t4(4,1) == 1))'] };
  const absent = { box: 'CD23', scene: 3011, cond: ['(progress < 41)'] };
  assert.deepEqual(doorBranches([first, second, absent], (door) => door.box), [second, absent]);
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

test('game over waits for the continue choice and selects its left entry', () => {
  const { gameOverMenu } = skill('recovery', {});
  assert.equal(gameOverMenu.active({ closeState: 0 }), false);
  assert.equal(gameOverMenu.ready({ closeState: 2 }), false);
  assert.equal(gameOverMenu.ready({ closeState: 3 }), true);
  assert.equal(gameOverMenu.decide({ choice: 1 }), 'left');
  assert.equal(gameOverMenu.decide({ choice: 0 }), 'a');
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

test('armor targets model damage to the guard and the contact cooldown', () => {
  const fields = { 0xb1c: 240, 0xb20: 65, 0xb2c: 9, 0xb32: 2 };
  const read = (addr) => fields[addr - 0x02010000] ?? 0;
  const { armorCombatant } = skill('armor', { u16: read, s16: read });
  assert.deepEqual(armorCombatant(0x02010000), { boss: 'Armor', health: 240, alive: true, hitLock: 9, guard: 65 });
  fields[0xb32] = 0x80;
  assert.equal(armorCombatant(0x02010000).alive, false);
  assert.equal(armorCombatant(0x02010000).guard, 0);
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
