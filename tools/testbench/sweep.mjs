#!/usr/bin/env node
import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { parseArgs } from 'node:util';
import { writeCoverage } from './coverage.mjs';
import { menuFixtures } from './menus.mjs';
import { runIsolated, failed, summary } from './runner.mjs';
import { catalog, root } from './support.mjs';

const { values } = parseArgs({ options: {
  out: { type: 'string' }, filter: { type: 'string' }, 'no-rom': { type: 'boolean' },
  timeout: { type: 'string' }, help: { type: 'boolean', short: 'h' },
} });
if (values.help) {
  console.log('Usage: node tools/testbench/sweep.mjs [--filter REGEX] [--out DIR] [--no-rom] [--timeout SECONDS]');
  process.exit(0);
}
const out = path.resolve(values.out ?? path.join(root, 'build/testbench/sweep'));
const filter = values.filter ? new RegExp(values.filter, 'i') : null;
const timeout = values.timeout === undefined ? undefined : Number(values.timeout) * 1000;
if (timeout !== undefined && (!Number.isInteger(timeout) || timeout < 1000)) throw new RangeError('Timeout must be at least one second');
const modulePath = fileURLToPath(new URL('smoke.mjs', import.meta.url));
const fixtures = [
  ...catalog.filter(e => e.role === 'spawn').map(e => ({ id: e.key, name: e.name, parameters: { actor: e.key } })),
  ...menuFixtures.map(name => ({ id: name.replaceAll(/[^a-z0-9]+/gi, '-'), name, parameters: { menu: name } })),
];
const selected = fixtures.filter(f => !filter || filter.test(`${f.id} ${f.name}`));
if (!selected.length) throw new Error('No testbench cases matched the filter');
const cases = [], findings = [];
fs.mkdirSync(out, { recursive: true });
const resultFile = path.join(out, 'sweep.jsonl');
fs.writeFileSync(resultFile, '');
for (const f of selected) {
  if (fs.existsSync(path.join(root, 'build/STOP'))) break;
  const result = await runIsolated(modulePath, path.join(out, f.id), {
    parameters: f.parameters, rom: !values['no-rom'], timeout: timeout ?? (!values['no-rom'] && f.name.startsWith('pause/') ? 40_000 : 8_000),
  });
  cases.push({ id: f.id, name: f.name, result });
  fs.appendFileSync(resultFile, JSON.stringify(cases.at(-1)) + '\n');
  console.log(`${f.id} ${f.name} · ${summary(result)}`);
  if (failed(result)) {
    const reason = result.failure?.message ?? result.rom?.reason ?? JSON.stringify(result.rom.differences);
    findings.push(`- ${f.id} ${f.name} (${result.status === 'pass' ? 'ROM ' + result.rom.status : result.status}, ${result.phase ?? 'setup'}). ${reason.split('\n')[0]}`);
  }
}
const coverage = writeCoverage(cases, out);
const headline = `${coverage.percent.toFixed(2)}% in-context coverage (${coverage.hit}/${coverage.total} functions)`;
const counts = {};
for (const { result } of cases) {
  counts[result.status] = (counts[result.status] ?? 0) + 1;
  if (result.rom) counts[`ROM ${result.rom.status}`] = (counts[`ROM ${result.rom.status}`] ?? 0) + 1;
}
const tally = Object.entries(counts).map(([k, v]) => `${v} ${k}`).join(', ');
const status = `${cases.length}/${selected.length} cases · ${tally || 'no cases run'}`;
fs.writeFileSync(path.join(out, 'summary.md'), `# Testbench sweep\n\n${headline}\n\n${status}\n\n## Findings\n\n${findings.join('\n') || 'None.'}\n`);
console.log(`${headline} · ${status}\n${path.relative(root, out)}`);
process.exitCode = findings.length || cases.length !== selected.length ? 1 : 0;
