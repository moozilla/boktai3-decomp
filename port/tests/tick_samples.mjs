import assert from 'node:assert/strict';
import test from 'node:test';
import { compareSamples, readPressedKeys } from '../../tools/autopilot/lib/tick-samples.mjs';

const rom = Buffer.from([9, 12, 10]);
const portSamples = [...rom].map((byte) => Buffer.of(byte));
const fields = [{ name: 'scene', size: 1 }];
const frames = [16, 17, 18];

test('KEYINPUT decodes active-low button bits and ignores the unused bits', () => {
  const read = (value) => readPressedKeys({
    getUint16(address, littleEndian) {
      assert.equal(address, 0x04000130);
      assert.equal(littleEndian, true);
      return value;
    },
  });
  assert.equal(read(0x03ff), 0);
  assert.equal(read(0xfdf6), 0x0209);
});

test('a shifted controller read fails even with matching RAM and tick counts', () => {
  assert.equal(compareSamples(rom, portSamples, fields, frames, [0, 8, 0], [0, 8, 0]), null);
  assert.deepEqual(compareSamples(rom, portSamples, fields, frames, [0, 8, 0], [0, 0, 8]), {
    tick: 1, frame: 17, changes: { input: { port: 8, rom: 0 } },
  });
});

test('an earlier RAM difference takes precedence over input drift', () => {
  const samples = [Buffer.of(7), ...portSamples.slice(1)];
  assert.deepEqual(compareSamples(rom, samples, fields, frames, [0, 8, 0], [0, 0, 8]), {
    tick: 0, frame: 16, changes: { scene: { port: '07', rom: '09' } },
  });
});

test('a port poll beyond the fixture is an input difference', () => {
  assert.deepEqual(compareSamples(rom.subarray(0, 2), portSamples, fields, frames, [0, 8, 0], [0, 8]), {
    tick: 2, frame: 18, changes: { input: { port: 0, rom: null } },
  });
});
