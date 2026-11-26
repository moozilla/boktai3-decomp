import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';
import { root } from './support.mjs';

const EWRAM = 0x02000000, END = EWRAM + 0x40000;
const BUTTONS = ['A', 'B', 'SELECT', 'START', 'RIGHT', 'LEFT', 'UP', 'DOWN', 'R', 'L'];

export function stateReader(types) {
  const offset = (type, field) => types.types[`struct ${type}`]?.fields?.find(f => f.name === field)?.off;
  const players = types.globals.gPlayers?.addr, gs = types.globals.gGameState?.addr, vars = types.globals.gGameVariables?.addr;
  const position = offset('Player', 'position'), life = offset('GameState', 'life'), scene = offset('GameState', 'unk5A4');
  if ([players, gs, vars, position, life, scene].some(v => v === undefined))
    throw new Error('The agent metadata is missing the state fields needed for ROM comparison');
  return (memory, base) => {
    const word = a => memory.getUint32(a - base, true);
    const short = a => memory.getInt16(a - base, true);
    const p = word(players), game = word(gs), v = word(vars);
    const ewram = (a, size) => a >= EWRAM && a + size <= END;
    return {
      scene: ewram(game, scene + 4) ? word(game + scene) : null,
      story: ewram(v, 0x3e) ? short(v + 0x3c) : null,
      life: ewram(game, life + 2) ? short(game + life) : null,
      position: ewram(p, position + 6) ? ['x', 'y', 'z'].map((_, i) => short(p + position + i * 2)) : null,
    };
  };
}

export function harnessScript({ time, solar, solarLog, movie, ticks }, tickAddress) {
  const last = ticks.length - 1;
  const lines = ['trace off', `rtc ${Math.floor(Date.parse(time) / 1000)}`, `lux ${solar}`,
    `tick ${tickAddress.toString(16)}`, `tickdump end ${last}:${last + 1} 02000000:40000`];
  let at = 0, lux = solar;
  for (let i = 0; i < ticks.length;) {
    while (at < solarLog.length && solarLog[at][0] <= ticks[i]) {
      lux = solarLog[at++][1];
      lines.push(`lux ${lux}`);
    }
    const mask = movie[ticks[i]] || 0;
    let j = i + 1;
    while (j < ticks.length && (movie[ticks[j]] || 0) === mask && (at >= solarLog.length || ticks[j] < solarLog[at][0])) j++;
    const keys = BUTTONS.filter((_, bit) => mask & (1 << bit)).join('+') || 'NONE';
    lines.push(`holdt ${keys} ${j - i}`);
    i = j;
  }
  return lines.join('\n') + '\n';
}

export function verifyRom(result, out, { timeout = 30000 } = {}) {
  if (result.mutations.length) return { status: 'unreproducible', reasons: result.mutations };
  if (!result.replay?.state || !result.replay.ticks.length) return { status: 'unavailable', reason: 'No input ticks were recorded' };
  if (result.status !== 'pass') return { status: 'unavailable', reason: `Port case ended with ${result.status}` };
  const types = JSON.parse(fs.readFileSync(path.join(root, 'build/port/agent/types.json')));
  const map = stateReader(types);
  const romOut = path.join(out, 'rom');
  fs.mkdirSync(romOut, { recursive: true });
  const script = path.join(out, 'harness.txt');
  fs.writeFileSync(script, harnessScript(result.replay, types.functions.Input_UpdatePadStates & ~1));
  const privateHarness = path.join(root, 'build/studio/experiments/harness');
  const harness = process.env.BOKTAI3_HARNESS ?? (fs.existsSync(privateHarness) ? privateHarness : path.join(root, 'tools/emu/harness'));
  fs.rmSync(path.join(romOut, 'end.bin'), { force: true });
  const run = spawnSync(harness, [path.join(root, 'baserom.gba'), script, romOut], {
    cwd: root, env: { ...process.env, BOKTAI3_SAV: result.replay.save }, timeout, killSignal: 'SIGKILL', encoding: 'utf8',
  });
  const log = (run.stderr || '') + (run.stdout || '');
  fs.writeFileSync(path.join(romOut, 'harness.log'), log);
  for (const name of fs.readdirSync(romOut)) if (name.startsWith('total.')) fs.unlinkSync(path.join(romOut, name));
  if (run.error || run.status !== 0 || /holdt: \d+ of \d+ ticks/.test(log))
    return { status: run.error?.code === 'ETIMEDOUT' ? 'hang' : 'error', reason: /unknown command tick/.test(log)
      ? 'Replay needs a current harness. Run make -f tools/emu/Makefile build/studio/experiments/harness.'
      : run.error?.message || log.trim() || `harness exited ${run.status}` };
  if (!fs.existsSync(path.join(romOut, 'end.bin'))) return { status: 'error', reason: 'The ROM harness produced no EWRAM sample' };
  const bytes = fs.readFileSync(path.join(romOut, 'end.bin'));
  if (bytes.length !== END - EWRAM) return { status: 'error', reason: `Expected one EWRAM sample, got ${bytes.length} bytes` };
  const rom = map(new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength), EWRAM);
  const port = result.replay.state;
  const differences = {};
  for (const field of Object.keys(port)) {
    if (JSON.stringify(port[field]) !== JSON.stringify(rom[field])) differences[field] = { port: port[field], rom: rom[field] };
  }
  return { status: Object.keys(differences).length ? 'different' : 'match', tick: result.replay.ticks.length - 1,
    port, rom, differences };
}
