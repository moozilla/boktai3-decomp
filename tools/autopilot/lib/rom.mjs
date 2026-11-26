import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';

export const root = fileURLToPath(new URL('../../../', import.meta.url));
export const rom = path.join(root, 'baserom.gba');
export const clock = '2025-11-13T12:00:00Z';
export const lux = 232;
const keyNames = ['A', 'B', 'SELECT', 'START', 'RIGHT', 'LEFT', 'UP', 'DOWN', 'R', 'L'];

export function holdTicks(reads) {
  const lines = [];
  for (let i = 0; i < reads.length;) {
    let next = i + 1;
    while (next < reads.length && reads[next] === reads[i]) next++;
    const keys = keyNames.filter((_, bit) => reads[i] & (1 << bit)).join('+') || 'NONE';
    lines.push(`holdt ${keys} ${next - i}`);
    i = next;
  }
  return lines;
}

// keyTickPc: the keypad load in the tick function's prologue (BOKTAI3_KEY_TICK_PC in
// tools/emu/harness.c), so untraced `tick` replays keep mGBA's normal frame loop.
// Returns the harness warnings about ticks; a missed holdt throws.
export function runRom(script, out, { save, coverage = false, render = true, timeout = 90_000, keyTickPc } = {}) {
  const local = path.join(root, 'build/emu/harness');
  const harness = process.env.BOKTAI3_HARNESS || (fs.existsSync(local) ? local : path.join(root, 'tools/emu/harness'));
  const env = { ...process.env };
  delete env.BOKTAI3_SAV;
  delete env.BOKTAI3_NO_COVERAGE;
  delete env.BOKTAI3_HEADLESS;
  delete env.BOKTAI3_KEY_TICK_PC;
  if (save) env.BOKTAI3_SAV = path.resolve(save);
  if (!coverage) env.BOKTAI3_NO_COVERAGE = '1';
  if (!render) env.BOKTAI3_HEADLESS = '1';
  if (keyTickPc) env.BOKTAI3_KEY_TICK_PC = keyTickPc.toString(16);
  const run = spawnSync(harness, [rom, script, out], {
    env, timeout, encoding: 'utf8', maxBuffer: 1 << 25,
  });
  fs.writeFileSync(path.join(out, 'harness.log'), (run.stdout || '') + (run.stderr || ''));
  if (run.error) throw run.error;
  if (run.status !== 0) throw new Error(`ROM harness exited ${run.status ?? run.signal}. See ${out}/harness.log`);
  const missed = (run.stderr || '').split('\n').find((line) => /^holdt:/.test(line));
  if (missed) throw new Error(missed);
  return (run.stderr || '').split('\n').filter((line) => /ticks$|not reached|times in|error/i.test(line));
}
