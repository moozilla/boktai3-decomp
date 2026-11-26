import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import test from 'node:test';
import { replayAgainstRom } from '../../tools/autopilot/lib/rom-reference.mjs';

test('ROM samples are reusable only for identical complete replays', (t) => {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'boktai3-rom-reference-'));
  t.after(() => fs.rmSync(dir, { force: true, recursive: true }));
  const reference = path.join(dir, 'reference');
  const out = path.join(dir, 'out');
  fs.mkdirSync(reference);
  fs.mkdirSync(out);
  const ticks = Buffer.from([1, 2, 3]);
  const inputFiles = { rom: 'ROM', save: 'checkpoint', script: 'holdt A 3' };
  const input = Object.fromEntries(Object.entries(inputFiles).map(([name, data]) => {
    const file = path.join(dir, name);
    fs.writeFileSync(file, data);
    return [name, file];
  }));
  let replays = 0;
  const run = (dir, data = ticks) => {
    replays++;
    fs.writeFileSync(path.join(dir, 'ticks.bin'), data);
  };
  const capture = (data) => replayAgainstRom({ ...input, out: reference, expectedBytes: 3,
    run: () => run(reference, data) });
  const compare = () => replayAgainstRom({ ...input, out, reference, expectedBytes: 3,
    run: () => run(out) });

  assert.deepEqual(capture(), ticks);
  assert.deepEqual(compare(), ticks);
  assert.equal(replays, 1);

  for (const [name, data] of Object.entries(inputFiles)) {
    fs.appendFileSync(input[name], '!');
    const previous = replays;
    assert.deepEqual(compare(), ticks);
    assert.equal(replays, previous + 1, `changed ${name} needs a ROM replay`);
    fs.writeFileSync(input[name], data);
  }

  fs.writeFileSync(path.join(reference, 'ticks.bin'), Buffer.from([3, 2, 1]));
  assert.deepEqual(compare(), ticks);
  assert.equal(replays, 5);
  capture(Buffer.from([1, 2]));
  assert.equal(fs.existsSync(path.join(reference, 'reference.json')), false);
  assert.deepEqual(compare(), ticks);
  assert.equal(replays, 7);
});
