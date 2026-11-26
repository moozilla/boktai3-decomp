import fs from 'node:fs';
import path from 'node:path';
import { spawn } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import { root } from './support.mjs';

export async function runIsolated(modulePath, out, options = {}) {
  const { timeout = 60000, frameLimit = 12000, rom = true, romTimeout = 30000, parameters } = options;
  fs.mkdirSync(out, { recursive: true });
  for (const name of ['result.json', 'progress.json']) fs.rmSync(path.join(out, name), { force: true });
  const file = name => path.join(out, name);
  const proc = spawn(process.execPath, [fileURLToPath(new URL('worker.mjs', import.meta.url)), path.resolve(modulePath),
    path.resolve(out), JSON.stringify({ frameLimit, rom, romTimeout, parameters })], { cwd: root, stdio: ['ignore', 'pipe', 'pipe'] });
  const log = fs.createWriteStream(file('worker.log'));
  proc.stdout.pipe(log, { end: false });
  proc.stderr.pipe(log, { end: false });
  let timedOut = false;
  const timer = setTimeout(() => { timedOut = true; proc.kill('SIGKILL'); }, timeout);
  const closed = await new Promise(resolve => {
    proc.on('error', error => resolve({ error }));
    proc.on('close', (code, signal) => resolve({ code, signal }));
  });
  clearTimeout(timer);
  await new Promise(resolve => log.end(resolve));
  const partial = fs.existsSync(file('result.json')) ? JSON.parse(fs.readFileSync(file('result.json'))) : null;
  if (partial && !timedOut) return partial;
  const progress = fs.existsSync(file('progress.json')) ? JSON.parse(fs.readFileSync(file('progress.json'))) : { phase: 'boot' };
  const kind = timedOut ? 'hang' : closed.signal ? 'crash' : 'error';
  const failure = { kind, message: timedOut ? `Wall deadline expired after ${timeout} ms` :
    closed.error?.message ?? `Scenario subprocess stopped with ${closed.signal ?? closed.code}` };
  const result = { ...partial, ...progress, status: kind, failure, coverage: partial?.coverage ?? [] };
  fs.writeFileSync(file('result.json'), JSON.stringify(result, null, 2) + '\n');
  return result;
}

export function failed(result) {
  return result.status !== 'pass' || ['different', 'error', 'hang', 'unavailable'].includes(result.rom?.status);
}

export function summary(result) {
  const parts = [result.status, result.rom ? `ROM ${result.rom.status}` : null, `${result.coverage?.length ?? 0} functions`];
  const reason = result.failure?.message ?? result.rom?.reason ?? result.rom?.reasons?.join(', ');
  return parts.filter(Boolean).join(' · ') + (reason ? ` · ${reason}` : '');
}
