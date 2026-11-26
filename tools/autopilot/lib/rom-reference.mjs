import { createHash } from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';

const digest = (bytes) => createHash('sha256').update(bytes).digest('hex');
const fileDigest = (file) => digest(fs.readFileSync(file));

function readReference(dir, inputs, expectedBytes) {
  try {
    const record = JSON.parse(fs.readFileSync(path.join(dir, 'reference.json'), 'utf8'));
    if (Object.entries(inputs).some(([key, value]) => record?.[key] !== value)) return;
    const ticks = fs.readFileSync(path.join(dir, 'ticks.bin'));
    if (ticks.length === expectedBytes && record.ticks === digest(ticks)) return ticks;
  } catch (error) {
    if (!(error instanceof SyntaxError) && error.code !== 'ENOENT') throw error;
  }
}

// An explicit reference is an earlier observation of this ROM, save and replay.
// Comparing against it still runs the port and checks every sampled tick.
export function replayAgainstRom({ rom, save, script, out, reference, expectedBytes, run }) {
  const manifest = path.join(out, 'reference.json');
  const dump = path.join(out, 'ticks.bin');
  fs.rmSync(manifest, { force: true });
  fs.rmSync(dump, { force: true });
  const inputs = {
    version: 1,
    rom: fileDigest(rom),
    save: save ? fileDigest(save) : null,
    script: fileDigest(script),
  };
  const reused = reference && expectedBytes > 0 && readReference(reference, inputs, expectedBytes);
  if (reused) {
    fs.writeFileSync(dump, reused);
    fs.writeFileSync(path.join(out, 'harness.log'), `Reused ROM samples from ${reference}\n`);
  } else {
    run();
  }
  const ticks = reused || fs.readFileSync(dump);
  if (expectedBytes > 0 && ticks.length === expectedBytes)
    fs.writeFileSync(manifest, JSON.stringify({ ...inputs, ticks: digest(ticks) }) + '\n');
  return ticks;
}
