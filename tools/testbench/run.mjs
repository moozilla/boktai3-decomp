#!/usr/bin/env node
import path from 'node:path';
import { parseArgs } from 'node:util';
import { runIsolated, failed, summary } from './runner.mjs';
import { root } from './support.mjs';

const { values, positionals } = parseArgs({ allowPositionals: true, options: {
  out: { type: 'string' }, 'no-rom': { type: 'boolean' }, frames: { type: 'string' }, timeout: { type: 'string' }, case: { type: 'string' },
  help: { type: 'boolean', short: 'h' },
} });
if (values.help || positionals.length !== 1) {
  console.log('Usage: node tools/testbench/run.mjs SCENARIO.mjs [--case NAME] [--out DIR] [--no-rom] [--frames COUNT] [--timeout SECONDS]');
  process.exit(positionals.length === 1 || values.help ? 0 : 2);
}
function positive(value, fallback) {
  const count = value === undefined ? fallback : Number(value);
  if (!Number.isInteger(count) || count < 1) throw new RangeError('Frame and timeout limits must be positive integers');
  return count;
}
const file = path.resolve(positionals[0]);
const out = path.resolve(values.out ?? path.join(root, 'build/testbench', path.basename(file, '.mjs')));
const result = await runIsolated(file, out, {
  frameLimit: positive(values.frames, 12000), timeout: positive(values.timeout, 60) * 1000, rom: !values['no-rom'],
  parameters: values.case,
});
console.log(summary(result));
if (result.rom?.differences && Object.keys(result.rom.differences).length) console.log(JSON.stringify(result.rom.differences, null, 2));
for (const file of result.artifacts ?? []) console.log(path.relative(root, file));
console.log(path.relative(root, path.join(out, 'result.json')));
process.exitCode = failed(result) ? 1 : 0;
