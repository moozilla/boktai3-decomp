import fs from 'node:fs';
import path from 'node:path';
import { root } from './support.mjs';

export function reportCoverage(cases, inventory) {
  const byAddress = new Map();
  for (const { id, result } of cases) for (const address of result.coverage ?? []) {
    if (!byAddress.has(address)) byAddress.set(address, new Set());
    byAddress.get(address).add(id);
  }
  const functions = {}, uncovered = {};
  let total = 0, hit = 0;
  for (const { name: category, units } of inventory.categories) for (const { name: unitName, functions: entries } of units) {
    const unit = `${category}/${unitName}`;
    for (const [name, key] of entries) {
      const seen = [...(byAddress.get(parseInt(key, 16)) ?? [])];
      functions[key] = { name, unit, scenarios: seen };
      total++;
      if (seen.length) hit++;
      else (uncovered[unit] ??= []).push({ name, address: key });
    }
  }
  return { hit, total, percent: total ? 100 * hit / total : 0, functions, uncovered };
}

export function writeCoverage(cases, out) {
  const report = reportCoverage(cases, JSON.parse(fs.readFileSync(path.join(root, 'PROGRESS.json'))));
  fs.writeFileSync(path.join(out, 'coverage.json'), JSON.stringify(report, null, 2) + '\n');
  const lines = [];
  for (const [unit, functions] of Object.entries(report.uncovered)) {
    lines.push(`## ${unit}`, '', ...functions.map(f => `- \`${f.address}\` ${f.name}`), '');
  }
  fs.writeFileSync(path.join(out, 'unreached.md'), '# Functions the sweep did not reach\n\n' + lines.join('\n'));
  return report;
}
