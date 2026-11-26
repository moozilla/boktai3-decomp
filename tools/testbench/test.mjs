import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import { test } from 'node:test';
import { reportCoverage } from './coverage.mjs';
import { registryBlock, scriptVariable } from './gcl.mjs';
import { menuFixtures } from './menus.mjs';
import { harnessScript } from './replay.mjs';
import { runIsolated } from './runner.mjs';
import { actorEntry, catalog, readCsv, root } from './support.mjs';

const wasm = fs.existsSync(path.join(root, 'build/port/game/game.wasm')) &&
  fs.existsSync(path.join(root, 'build/port/agent/types.json'));
const harness = fs.existsSync(path.join(root, 'build/studio/experiments/harness')) || process.env.BOKTAI3_HARNESS;
const artifacts = fs.mkdtempSync(path.join(root, 'build/testbench-tests-'));
const example = (name, opts) => runIsolated(path.join(root, 'tools/testbench/examples', name + '.mjs'),
  path.join(artifacts, name + (typeof opts?.parameters === 'string' ? '-' + opts.parameters.replaceAll(/[^a-z0-9]+/gi, '-') : '')),
  { rom: false, timeout: 25000, ...opts });
function passed(result) { assert.equal(result.status, 'pass', result.failure?.message); return result; }

test('catalog names resolve by game name, registry key and C callback', () => {
  assert.equal(actorEntry('Ghoul').key, '05A5');
  assert.equal(actorEntry('ボク').key, '05A5');
  assert.equal(actorEntry(0x05a5).init, 'Ghoul_Init');
  assert.equal(actorEntry('Wooden Crate').key, 'F04B');
  assert.equal(actorEntry('Collection Book').key, 'A2DB');
  assert.equal(actorEntry('Projectile_SpawnAtPosition').key, 'native:scene-projectile');
  const entries = new Set(catalog.map(e => e.key));
  assert.equal(entries.size, catalog.length);
  for (const [key] of readCsv(path.join(root, 'data/actor_registry.csv')).slice(1)) assert(entries.has(key));
});

test('registry payload matches the script VM packet and keyword layout', () => {
  assert.deepEqual([...registryBlock('chara', '05A5', [{ hash: 0x7000 }])],
    [0x6a, 0x06, 0x99, 6, 6, 0xa5, 5, 6, 0, 0x70, 0, 0]);
  assert.deepEqual([...registryBlock('command', '05A5', [], { p: [-257] })],
    [0x6c, 0x45, 0xb7, 3, 6, 0xa5, 5, 0x54, 0x70, 1, 0xff, 0xfe, 0, 0]);
  assert.throws(() => registryBlock('command', '05A5', [], { bad: 10 }), /keyword/);
  assert.deepEqual(scriptVariable('r708:0x6e.1:t4'), { bank: 'r708', offset: 0x6e, bit: 1, type: 4, size: 1 });
  assert.throws(() => scriptVariable('r700:0x400:t1'), /range/);
});

test('movies are encoded in game ticks and respect sensor changes', () => {
  const script = harnessScript({
    time: '2025-11-13T14:00:00Z', solar: 232, solarLog: [[3, 9]],
    movie: [0, 1, 1, 1, 0], ticks: [1, 3, 4],
  }, 0x08219b24);
  assert.match(script, /tick 8219b24\ntickdump end 2:3 02000000:40000\nholdt A 1\nlux 9\nholdt A 1\nholdt NONE 1/);
});

test('coverage uses a per-function denominator and groups misses by unit', () => {
  const coverage = reportCoverage([{ id: 'A', result: { coverage: [0x08000000, 0x08000000] } }], {
    categories: [{ name: 'Engine', units: [{ name: 'Unit', functions: [['Reached', '08000000'], ['Unreached', '08000004']] }] }],
  });
  assert.equal(coverage.percent, 50);
  assert.deepEqual(coverage.functions['08000000'].scenarios, ['A']);
  assert.deepEqual(coverage.uncovered['Engine/Unit'], [{ name: 'Unreached', address: '08000004' }]);
});

test('stage equipment and flags, damage a typed Ghoul and render it', { skip: !wasm }, async () => {
  const result = passed(await example('ghoul', { rom: true }));
  assert.equal(result.value.maxHp, 80);
  assert(result.value.afterInput < result.value.maxHp);
  assert(result.value.afterPlayer < result.value.playerHp);
  assert(result.value.after < result.value.afterInput);
  assert(result.coverage.includes(0x0808e1d4));
  assert.equal(result.rom.status, 'unreproducible');
  const png = fs.readFileSync(result.artifacts[0]);
  assert.equal(png.subarray(1, 4).toString(), 'PNG');
  assert.equal(png.readUInt32BE(16), 240);
  assert.equal(png.readUInt32BE(20), 160);
});

test('crate, projectile and particle occupy typed slots, transient slots expire', { skip: !wasm }, async () => {
  const result = passed(await example('props-and-projectiles'));
  assert(result.coverage.includes(0x081fd01c));
  assert(result.coverage.includes(0x0812d7d0));
});

test('warp, script procedure, C call and compiled C snippet', { skip: !wasm }, async () => {
  const result = passed(await example('script-and-c'));
  assert.equal(result.value, 9);
  assert(result.mutations.some(v => /procedure 945/.test(v)));
  assert(result.mutations.includes('C snippet'));
});

test('every menu fixture has a live update callback', { skip: !wasm }, async () => {
  for (const parameters of menuFixtures) {
    const result = await example('menu', { parameters });
    assert.equal(result.status, 'pass', `${parameters}: ${result.failure?.message}`);
    assert.equal(result.value.name, parameters);
  }
});

test('save and input replay agrees with the original ROM', { skip: !wasm || !harness }, async () => {
  const result = passed(await example('replay', { rom: true }));
  assert.equal(result.rom.status, 'match', JSON.stringify(result.rom));
});

test('Solar Bank runs in its own scene and agrees with the original ROM', { skip: !wasm || !harness }, async () => {
  const result = passed(await example('solar-bank', { rom: true }));
  assert(result.coverage.includes(0x081bc27c));
  assert.equal(result.rom.status, 'match', JSON.stringify(result.rom));
});

test('frame budget terminates a scenario inside VBlank', { skip: !wasm }, async () => {
  const result = await example('replay', { frameLimit: 500 });
  assert.equal(result.status, 'hang');
  assert.match(result.failure.message, /Frame budget exceeded/);
});
