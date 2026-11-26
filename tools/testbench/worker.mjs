import fs from 'node:fs';
import path from 'node:path';
import { pathToFileURL } from 'node:url';
import { execute } from './scenario.mjs';
import { verifyRom } from './replay.mjs';

const [modulePath, out, opts] = process.argv.slice(2);
const { parameters, frameLimit, rom, romTimeout } = JSON.parse(opts);
const write = (name, object) => {
  const file = path.join(out, name);
  fs.writeFileSync(file + '.new', JSON.stringify(object, null, 2) + '\n');
  fs.renameSync(file + '.new', file);
};

try {
  const mod = await import(pathToFileURL(modulePath));
  const definition = typeof mod.default === 'function' ? mod.default(parameters) : mod.default;
  const result = await execute(definition, { out, frameLimit, onProgress: info => write('progress.json', info) });
  write('movie.json', result.replay);
  const { replay, ...summary } = result;
  write('result.json', summary);
  summary.rom = rom ? verifyRom(result, out, { timeout: romTimeout }) : { status: 'disabled' };
  write('result.json', summary);
} catch (error) {
  write('result.json', { status: 'error', failure: { kind: 'error', message: error.message, stack: error.stack }, coverage: [] });
  process.exitCode = 1;
}
