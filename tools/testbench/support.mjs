import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

export const root = fileURLToPath(new URL('../../', import.meta.url));
export const python = path.join(root, 'build/venv/bin/python');
export const defaultSave = path.join(root, 'tests/saves/ShinBok2.sav');

export function readCsv(file) {
  return fs.readFileSync(file, 'utf8').trim().split(/\r?\n/).map(line =>
    [...line.matchAll(/(?:^|,)(?:"((?:[^"]|"")*)"|([^,]*))/g)]
      .map(([, quoted, plain]) => quoted === undefined ? plain : quoted.replaceAll('""', '"')));
}

export const catalog = JSON.parse(fs.readFileSync(new URL('catalog.json', import.meta.url))).entries;
const normalize = name => String(name).normalize('NFKC').toLowerCase().replace(/[\s_-]/g, '');

export function actorEntry(name) {
  const key = typeof name === 'number' ? name.toString(16).padStart(4, '0') : String(name).replace(/^0x/i, '');
  const exact = catalog.find(entry => entry.key.toLowerCase() === key.toLowerCase());
  if (exact) return exact;
  const matches = catalog.filter(e => [e.name, ...e.aliases].some(n => normalize(n) === normalize(name)));
  if (matches.length === 1) return matches[0];
  if (matches.length) throw new Error(`Ambiguous actor "${name}": ${matches.map(e => e.key).join(', ')}`);
  throw new Error(`Unknown actor "${name}"`);
}

const items = readCsv(path.join(root, 'data/items.csv')).slice(1);
export function itemId(category, name) {
  const label = normalize(name === 'Solar Nut' ? 'Solar Fruit' : name);
  const item = items.find(([kind, id, , jp, en]) => kind === category &&
    (typeof name === 'number' ? parseInt(id, 16) === name : [jp, en].some(value => normalize(value) === label)));
  if (!item) throw new Error(`Unknown ${category} "${name}"`);
  return parseInt(item[1], 16);
}
