import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';
import { openSession, png } from '../../port/agent/session.mjs';
import { actorEntry, defaultSave, itemId, python, root } from './support.mjs';
import { registryBlock, scriptVariable } from './gcl.mjs';
import { stateReader } from './replay.mjs';
import { nativeActors } from './native.mjs';

// Free space in the port, past the C snippet's data and shadow stack.
const SCRATCH = 0x02880000;
const FIXTURE_END = 0x02980000;
const ARENA = { scene: 2776, entrance: 7, story: 25 };
const LENSES = { sol: 0, luna: 1, flame: 2, frost: 3, cloud: 4, earth: 5, star: 6, astro: 7, dark: 8 };
const SUN_INTENSITIES = [0, 3, 9, 17, 28, 42, 58, 76, 98, 124, 140];

export function scenario(options, body) {
  if (typeof body !== 'function') throw new TypeError('A scenario needs a generator function');
  return { options: { ...options }, body };
}

export class FrameLimit extends Error {}

function clock(time = '12:00') {
  const iso = /^\d{2}:\d{2}(?::\d{2})?$/.test(time)
    ? `2025-11-13T${time.length === 5 ? time + ':00' : time}Z` : time;
  if (!Number.isFinite(Date.parse(iso))) throw new Error(`Invalid RTC time ${time}`);
  return iso;
}

function prepare(options, out) {
  const settings = { ...(options.empty ? ARENA : {}), ...options };
  const source = path.resolve(root, settings.save ?? defaultSave);
  const stage = settings.stage ?? 'save';
  if (!['save', 'warp'].includes(stage)) throw new Error('stage must be "save" or "warp"');
  const fields = Object.fromEntries(['story', 'scene', 'entrance'].filter(k => settings[k] !== undefined).map(k => [k, settings[k]]));
  const flags = Object.entries(settings.flags ?? {}).map(([key, value]) => ({ ...scriptVariable(key), value }));
  const saveFlags = stage === 'save' ? flags.filter(f => f.bank === 'r708' || f.bank === 'gs' && f.offset + f.size <= 0x858) : [];
  const pokes = flags.filter(f => !saveFlags.includes(f));
  let save = source;
  if (source.endsWith('.sgm') || stage === 'save' && (Object.keys(fields).length || saveFlags.length)) {
    save = path.join(out, 'fixture.sav');
    execFileSync(python, [path.join(root, 'tools/testbench/fixture.py'), source, save], {
      input: JSON.stringify({ ...(stage === 'save' ? fields : {}), flags: saveFlags }), cwd: root, stdio: ['pipe', 'pipe', 'pipe'],
    });
  }
  if (settings.sun !== undefined && SUN_INTENSITIES[settings.sun] === undefined) throw new Error('sun must be 0 through 10');
  return { ...settings, save, stage, pokes, time: clock(settings.time), solar: settings.solar ?? 232 };
}

function observeAllocations(g, entry, act) {
  const tasks = [], registry = [], slots = [];
  let returned = 0;
  const pending = [];
  function allocation(result, list) {
    if (result >>> 0) list.push(result >>> 0);
  }
  const hooks = [
    g.on(parseInt(entry.address, 16), ([result]) => { returned = result; }, { exit: true }),
    g.on('Task_Create', ([result]) => allocation(result, tasks), { exit: true }),
    g.on('Task_LinkToPriorityList', ([address]) => allocation(address, tasks)),
    g.on('SceneActorRegistry_LinkActor', ([address]) => pending.push(address)),
    g.on('SceneActorRegistry_LinkActor', ([result]) => {
      const address = pending.pop();
      if (result && address) registry.push(address);
    }, { exit: true }),
  ];
  for (const [name, type] of [
    ['WoodenCrate_AllocateSlot', 'struct WoodenCrate'],
    ['FloorSwitch_AllocateSlot', 'struct FloorSwitch'],
    ['Pool128_Allocate', 'struct RegionActor'],
    ['CollisionController_FindFreeSlot', 'struct CollisionSlot'],
    ['SceneIndicators_FindFreeSlot', 'struct SceneIndicator'],
    ['NamedObject_AllocateSlot', 'struct NamedObject'],
    ['SceneObjects_FindFreeSlot', 'struct SceneObject'],
  ]) {
    if (!(name in g.fn)) continue;
    hooks.push(g.on(name, ([address]) => {
      if (address) slots.push({ address, type: type in g.types.types ? type : 'u8[1]' });
    }, { exit: true }));
  }
  if (['Actor_Allocate', 'Actor_LinkToManager'].every(fn => fn in g.fn)) {
    const objects = new Set();
    hooks.push(g.on('Actor_Allocate', ([address]) => { if (address) objects.add(address); }, { exit: true }));
    hooks.push(g.on('Actor_LinkToManager', ([, address]) => {
      if (!objects.has(address)) return;
      const type = { BB34: 'struct SceneNpc', '8EA3': 'struct SceneCharacter' }[entry.key] ?? 'struct Actor';
      slots.push({ address, type: type in g.types.types ? type : 'u8[1]' });
    }));
  }
  try { return { result: act(), returned, tasks, registry, slots }; }
  finally { for (const id of hooks) g.off(id); }
}

function runtime(g, host, settings, out, mutations, progress) {
  const $ = g.$, artifacts = [];
  let nextId = 0x7000;
  let nextScratch = SCRATCH + 0x100;
  const mark = op => mutations.add(op);
  const put = (address, data) => new Uint8Array(host.memory.buffer, address, data.length).set(data);
  const allocate = size => {
    const address = nextScratch;
    nextScratch += Math.ceil(size / 4) * 4;
    if (nextScratch > FIXTURE_END) throw new Error('Testbench fixture memory exhausted');
    return address;
  };
  const describe = (address, type, kind, entry = null) => {
    const view = g.at(address, type), base = kind === 'registry' ? view.$cast('struct RoomActor') : view;
    return {
      address, type, kind, entry, view, base,
      get hp() { return kind === 'player' ? base.currentStatA : base.health; },
      get maxHp() { return kind === 'player' ? base.maximumStatA : base.defaultHealth ?? base.maxHealth; },
      get position() {
        const p = kind === 'registry' ? base.body.position : base.position ?? base.body?.position ?? base.sprite?.position ?? base;
        return p && typeof p === 'object' && ['x', 'y', 'z'].every(axis => axis in p) ? p : null;
      },
    };
  };
  const words = (descriptor, index, value) => {
    const { bank, offset, bit, size, type } = descriptor;
    const base = bank === 'gs' ? $.gGameState.$addr : bank === 'r700' ? $.gScriptLocalVariables : $.gGameVariables;
    const address = base + offset;
    const raw = (type << 24) | (bit << 16);
    if (!base || !Number.isInteger(index) || index < 0) throw new Error('Invalid script variable address or index');
    const stride = type === 8 ? 4 : size;
    const end = offset + (type === 4 ? ((index + bit) >> 3) + 1 : index * stride + size);
    if (end > (bank === 'gs' ? $.gGameState.$size : 0x400)) throw new Error('Script variable index out of range');
    if (value === undefined) {
      g.fn.Script_ReadTypedVariable(address, raw, index, SCRATCH);
      return g.s32(SCRATCH);
    }
    g.fn.Script_WriteTypedVariable(address, raw, index, value);
  };
  const reg = (entry, mode, args, options) => {
    const packet = registryBlock(mode, entry.key, args, options);
    const address = allocate(packet.length);
    const pc = $.gScriptVm.pc;
    put(address, packet);
    try { return g.fn.Script_RunBlock(address, 0); }
    finally { $.gScriptVm.pc = pc; }
  };
  const s = {
    g, $, host, artifacts,
    get player() { return $.gPlayers[0] ? describe($.gPlayers[0].$addr, 'struct Player', 'player') : null; },
    get frame() { return host.frame; },
    where() { return g.lib.where(); },
    run(frames, keys = 0) {
      if (!Number.isInteger(frames) || frames < 0) throw new RangeError('Frame count must be a nonnegative integer');
      return g.press(keys, frames);
    },
    press(keys, frames = 1) { return s.run(frames, keys); },
    tap(keys, settle = 6) { return (function* () { yield s.press(keys, 2); yield s.run(settle); })(); },
    until(test, frames = 300) { return g.until(test, frames); },
    *waitFor(test, frames, description) {
      if (!(yield s.until(test, frames))) throw new FrameLimit(`Timed out waiting for ${description}`);
    },
    *atTick(fn, frames = 90) {
      let running = false, done = false, value, failure;
      const id = g.on('TaskSystem_RunFrame', () => {
        if (running || done) return;
        running = true;
        try { value = fn(); } catch (error) { failure = error; }
        done = true;
      });
      try { yield* s.waitFor(() => done, frames, 'scheduler entry'); }
      finally { g.off(id); }
      if (failure) throw failure;
      return value;
    },
    call(name, ...args) {
      if (args.length > 12) throw new RangeError('The port call adapter supports at most 12 argument words');
      mark(`C call ${name}`);
      return g.call(name, ...args);
    },
    c(code, includes = []) {
      mark('C snippet');
      const file = execFileSync(python, [path.join(root, 'port/agent/snippet.py'), '--path',
        ...includes.flatMap(h => ['-i', h])], { input: code, encoding: 'utf8', cwd: root, stdio: ['pipe', 'pipe', 'pipe'] }).trim();
      const output = [];
      const env = { memory: host.memory, port_icall: host.exports.port_icall,
        agent_print: (ptr, length) => output.push(Buffer.from(g.bytes(ptr, length)).toString('latin1')) };
      for (const key of Object.keys(host.exports)) if (key.startsWith('port_bus_')) env[key] = host.exports[key];
      const instance = new WebAssembly.Instance(new WebAssembly.Module(fs.readFileSync(file)), { env });
      return { result: instance.exports.snippet_main() >>> 0, output: output.join('') };
    },
    callproc(id, ...args) {
      if (!Number.isInteger(id) || id < 1 || id > $.gScriptProcedureTable.count) throw new RangeError(`Unknown procedure ${id}`);
      if (args.length > 16) throw new RangeError('A script procedure takes at most 16 arguments');
      mark(`script procedure ${id}`);
      const pc = $.gScriptVm.pc;
      g.u32(SCRATCH, args.length);
      g.u32(SCRATCH + 4, SCRATCH + 8);
      args.forEach((value, index) => g.u32(SCRATCH + 8 + 4 * index, value));
      try { return g.fn.Script_Call(id, args.length ? SCRATCH : 0); }
      finally { $.gScriptVm.pc = pc; }
    },
    *warp(scene, entrance, frames = 600) {
      const before = s.where().scene;
      yield* s.atTick(() => s.callproc(945, scene, entrance, 0));
      yield s.run(40);
      yield* s.waitFor(() => s.where().scene === scene && (settings.field === false || s.player !== null), frames,
        `scene ${scene} from scene ${before}`);
      yield s.run(30);
    },
    variable(key, index = 0) { return words(scriptVariable(key), index); },
    setVariable(key, value, index = 0) {
      mark(`script variable ${key}`);
      words(scriptVariable(key), index, value);
    },
    setStageVariable(field) {
      mark(`script variable ${field.bank}:${field.offset}`);
      words(field, 0, field.value);
    },
    setSolar(raw) {
      if (!Number.isInteger(raw) || raw < 0 || raw > 255) throw new RangeError('Solar sensor reading must be 0 through 255');
      host.setSolar(raw);
    },
    typed(address, type) { return g.at(address, type); },
    actor(view, kind, { position, isLive } = {}) {
      const actor = describe(view.$addr, view.$type, kind);
      if (position) Object.defineProperty(actor, 'position', { get: () => position.split('.').reduce((v, k) => v?.[k], view) });
      if (isLive) actor.isLive = isLive;
      return actor;
    },
    allocate(type, fields = {}) {
      mark(`injected ${type}`);
      const view = g.at(allocate(g.sizeof(type)), type);
      for (const [key, value] of Object.entries(fields)) view[key] = value;
      return view;
    },
    place(actor, position) {
      if (!actor?.position) throw new Error('Actor has no discovered position; use its typed view');
      mark('placement poke');
      for (const [axis, value] of Object.entries(position)) {
        if (!['x', 'y', 'z'].includes(axis) || !Number.isInteger(value) || value < -32768 || value > 32767)
          throw new Error(`Invalid position component ${axis}=${value}`);
      }
      if (actor.kind === 'registry' && actor.base.body.id) {
        const vector = ['x', 'y', 'z'].map(axis => position[axis] ?? actor.position[axis]);
        reg(actorEntry('C7DC'), 'command', [], { n: actor.base.body.id, p: vector });
      } else for (const [axis, value] of Object.entries(position)) actor.position[axis] = value;
      return actor;
    },
    damage(actor, amount) {
      if (!Number.isInteger(amount) || amount <= 0 || amount > 32767) throw new RangeError('damage must be 1 through 32767');
      if (!Number.isInteger(actor?.hp)) throw new Error('Actor has no discovered health');
      mark('forced damage');
      if (actor.kind === 'player') {
        g.fn.Player_SubtractLifeNonlethal(actor.view, amount);
      } else if (actor.kind === 'registry') {
        actor.base.reactionSoundVariant = amount;
        g.fn.SceneActorReaction_ApplyCategoryDamage(actor.view);
      } else {
        actor.base.health = Math.max(0, actor.hp - amount);
      }
      return actor.hp;
    },
    state(actor, fields) {
      mark('actor state poke');
      for (const [name, value] of Object.entries(fields)) {
        if (!(name in actor.base)) throw new Error(`${actor.type} has no field ${name}`);
        actor.base[name] = value;
      }
    },
    command(name, options = {}, args = []) {
      const entry = actorEntry(name);
      mark(`registry command ${entry.key} (${entry.name})`);
      return reg(entry, 'command', args, options);
    },
    spawn(name, opts = {}) {
      const entry = actorEntry(name);
      if (entry.key in nativeActors) {
        progress.phase = `creating ${entry.name} (${entry.key})`;
        const actor = nativeActors[entry.key].spawn(s, opts);
        if (actor) Object.assign(actor, { entry, created: [actor] });
        progress.phase = `running ${entry.name} (${entry.key})`;
        return actor;
      }
      for (const { global, creator } of entry.requires ?? []) {
        if (!$[global]) s.spawn(creator);
      }
      const recipe = opts.recipe === false ? null : entry.recipes[opts.recipe ?? 0];
      const mode = opts.mode ?? recipe?.mode ?? 'chara';
      const id = { hash: nextId++ };
      const args = opts.args ?? (mode === 'chara' ? [opts.id ?? recipe?.args?.[0] ?? id, ...(recipe?.args?.slice(1) ?? [])] : recipe?.args ?? []);
      const options = { ...recipe?.options, ...entry.defaults, ...opts.options };
      if (entry.allocation === 'SceneActorRegistry_LinkActor') {
        options.N = opts.options?.N ?? [id];
        if (entry.parameters.A && options.A === undefined) options.A = [80, 0, 0, 2, 1026, 1];
        if (entry.parameters.a && options.a === undefined) options.a = [8, 12, 2, 30, 12, 64, 2, 50];
      }
      if (opts.x !== undefined || opts.y !== undefined || opts.z !== undefined) {
        const p = s.player?.position;
        const position = [opts.x ?? p?.x ?? 0, opts.y ?? p?.y ?? 0, opts.z ?? p?.z ?? 0];
        if (!Object.hasOwn(opts.options ?? {}, 'p') && (options.p?.length === 3 || entry.parameters.p?.values === 3)) options.p = position;
      }
      for (const letter of Object.keys(options)) if (options[letter] === null) delete options[letter];
      mark(`registry ${entry.key} (${entry.name})`);
      progress.phase = `creating ${entry.name} (${entry.key})`;
      const trackCount = entry.key === '8337' ? $.gDisplayTrackManager?.count : undefined;
      const allocation = observeAllocations(g, entry, () => reg(entry, mode, args, options));
      progress.phase = `running ${entry.name} (${entry.key})`;
      const actors = allocation.registry.map(address => {
        const concrete = `struct ${entry.callback.split('_')[0]}`;
        const type = g.types.params[entry.init] ?? (concrete in g.types.types ? concrete : 'struct RoomActor');
        return describe(address, type, 'registry', entry);
      });
      const live = new Set(g.lib.tasks().filter(t => !t.dead).map(t => Number(t.task)));
      const fallback = entry.existing && $[entry.existing];
      const taskAddresses = [...new Set([...allocation.tasks, allocation.returned, fallback?.$addr ?? fallback])];
      actors.push(...taskAddresses.filter(a => live.has(a)).map(address => {
        const task = g.at(address, 'struct Task');
        return describe(address, g.types.params[g.name(task.update)] ?? 'struct Task', 'task', entry);
      }));
      actors.push(...allocation.slots.map(a => describe(a.address, a.type, 'slot', entry)));
      if (trackCount !== undefined && $.gDisplayTrackManager.count > trackCount) {
        const type = 'struct DisplayTrack';
        const address = $.gDisplayTrackManager.entries.$addr + trackCount * g.sizeof(type);
        const track = s.actor(g.at(address, type), 'slot', { position: 'sprite.position' });
        track.entry = entry;
        actors.push(track);
      }
      const actor = actors.find(a => a.kind === 'registry' || a.kind === 'slot') ?? actors[0] ?? null;
      if (actor?.position && (opts.x !== undefined || opts.y !== undefined || opts.z !== undefined)) {
        s.place(actor, Object.fromEntries(['x', 'y', 'z'].filter(k => opts[k] !== undefined).map(k => [k, opts[k]])));
      }
      if (actor) Object.assign(actor, { created: actors, allocation, recipe: recipe?.proc,
        adopted: !allocation.tasks.includes(actor.address) && actor.kind === 'task' });
      return actor;
    },
    expect(actual, label = 'value') {
      return {
        equals(expected) { assert.deepStrictEqual(actual, expected, label); },
        below(expected) { assert(actual < expected, `${label}: expected ${actual} < ${expected}`); },
        above(expected) { assert(actual > expected, `${label}: expected ${actual} > ${expected}`); },
        truthy() { assert(actual, label); },
      };
    },
    *screenshot(name = 'screen') {
      if (!/^[a-z0-9_-]+$/i.test(name)) throw new Error('Screenshot names use letters, numbers, underscores and dashes');
      host.setRender(true);
      try {
        yield s.run(2);
        const file = path.join(out, `${name}.png`);
        fs.writeFileSync(file, png(host.pixels()));
        artifacts.push(file);
        return file;
      } finally { host.setRender(!!settings.render); }
    },
  };
  return s;
}

function equip(s, equipment) {
  const g = s.g, gs = s.$.gGameState;
  if (equipment.sword !== undefined) {
    const id = itemId('sword', equipment.sword);
    let index = Array.from({ length: 16 }, (_, i) => i).find(i => g.fn.Inventory_GetEquipmentId(i) === id);
    if (index === undefined) {
      index = Array.from({ length: 16 }, (_, i) => i).find(i => g.fn.Inventory_GetEquipmentId(i) === 255);
      if (index === undefined) throw new Error('Equipment inventory is full');
      // A zeroed descriptor with its ID in byte 0 gives an unmodified sword.
      g.u32(SCRATCH, id); g.u32(SCRATCH + 4, 0);
      g.u32(SCRATCH + 8, 0); g.u32(SCRATCH + 12, 0);
      g.fn.Inventory_SetEquipmentEntry(index, SCRATCH);
    }
    const assigned = Array.from(gs.inventory).slice(0, 8).indexOf(index);
    if (assigned < 0) gs.inventory[0] = index;
    gs.unk50 = Math.max(0, assigned);
  }
  if (equipment.lens !== undefined) {
    const id = typeof equipment.lens === 'number' ? equipment.lens : LENSES[equipment.lens.toLowerCase().replace(/ lens$/, '')];
    if (!Number.isInteger(id) || id < 0 || id > 10) throw new Error(`Unknown lens "${equipment.lens}"`);
    let index = Array.from(gs.unk828).indexOf(id);
    if (index < 0) index = Array.from(gs.unk828).indexOf(-1);
    if (index < 0) throw new Error('Lens inventory is full');
    gs.unk828[index] = id;
    gs.slots88[4] = index;
    gs.actionAuxiliarySlot = 0;
  }
  s.call('Player_RefreshActionEquipment', s.player.view);
}

export async function execute(definition, { out = path.join(root, 'build/testbench/run'), print = () => {}, frameLimit = 12000,
  onProgress = () => {} } = {}) {
  out = path.resolve(out);
  fs.mkdirSync(out, { recursive: true });
  const settings = prepare(definition.options, out);
  const mutations = new Set(settings.replay === false ? ['scenario opted out of ROM replay'] : []);
  let phase = 'boot';
  const progress = { get phase() { return phase; }, set phase(value) { phase = value; onProgress({ phase, frame: session?.frame ?? 0 }); } };
  const logs = [];
  const ticks = [];
  let session, s, value, failure, start, lastTick, startedCoverage = false;
  try {
    session = await openSession({ ...settings, name: 'testbench', print: line => {
      logs.push(line);
      if (logs.length > 100) logs.shift();
      print(line);
    }, onFrame(frame) {
      if (frame > frameLimit) throw new FrameLimit(`Frame budget exceeded (${frame} > ${frameLimit})`);
    }, onWrite(address) {
      if (address >= 0x02000000 && address < 0x02040000 || address >= 0x03000000 && address < 0x08000000)
        mutations.add('typed or raw RAM write');
    }, onBoot(g) {
      const read = stateReader(g.types);
      const memory = new DataView(session.memory.buffer);
      if (settings.coverageFrom) g.on(settings.coverageFrom, (_args, info) => {
        session.setCoverage(true, { clear: true });
        session.coverageMap()[(info.addr - 0x08000000) >> 1] = 1;
        startedCoverage = true;
      }, { once: true });
      g.on('Input_UpdatePadStates', () => {
        ticks.push(session.frame);
        lastTick = read(memory, 0);
      });
    } });
    if (!session.types.globals.gGameState) throw new Error('Generate port types with port/agent/gametypes.py');
    value = session.run(function* (g, host) {
      s = runtime(g, host, settings, out, mutations, progress);
      yield* g.lib.continueSave();
      if (!s.player && settings.field !== false) throw new FrameLimit('Battery Continue did not reach a playable field');
      progress.phase = 'stage';
      if (settings.stage === 'warp') {
        if (settings.story !== undefined) s.setVariable('r708:0x3c:t1', settings.story);
        for (const f of settings.pokes.filter(f => f.bank !== 'r700')) s.setStageVariable(f);
        if (settings.scene !== undefined) yield* s.warp(settings.scene, settings.entrance ?? 1);
      }
      for (const f of settings.pokes.filter(f => settings.stage === 'save' || f.bank === 'r700')) s.setStageVariable(f);
      if (settings.scene !== undefined && s.where().scene !== settings.scene)
        throw new Error(`Expected scene ${settings.scene}, game reached ${s.where().scene}`);
      if (settings.sun !== undefined) {
        const apply = () => {
          s.$.gGameState.unk874 = SUN_INTENSITIES[settings.sun];
          s.$.gGameState.unk876 = settings.sun;
        };
        mutations.add('forced solar gauge');
        apply();
        g.on('Solar_UpdateTask', apply, { exit: true });
      }
      if (settings.equip) equip(s, settings.equip);
      for (const [name, count] of Object.entries(settings.items ?? {})) {
        if (!Number.isInteger(count) || count < 0 || count > 16) throw new Error(`Invalid count for ${name}`);
        const id = itemId('item', name);
        for (let i = 0; i < count; i++) if (!s.call('Inventory_AddItem', id, 0)) throw new Error(`Could not add ${name}`);
      }
      start = s.where();
      if (!startedCoverage) host.setCoverage(true, { clear: true });
      progress.phase = 'scenario';
      const result = definition.body(s);
      if (!result || typeof result.next !== 'function') throw new Error('Scenario body must be a synchronous generator (function*)');
      return yield* result;
    });
    if (session.frame > frameLimit) throw new FrameLimit(`Frame budget exceeded (${session.frame} > ${frameLimit})`);
    if (logs.some(line => line.startsWith('[fatal]'))) throw new WebAssembly.RuntimeError('The port reported a fatal error');
  } catch (error) {
    failure = { kind: error instanceof FrameLimit ? 'hang' : error.code === 'ERR_ASSERTION' ? 'assertion'
      : error instanceof WebAssembly.RuntimeError ? 'crash' : 'error', message: error.message, stack: error.stack };
  }
  let end = null;
  try { end = s?.where() ?? null; } catch { /* Preserve the original failure after memory corruption. */ }
  const addresses = session?.types.functions ?? {};
  const bitmap = session?.exports ? session.coverageMap() : [];
  const coverage = [...new Set(Object.values(addresses).map(a => a & ~1))]
    .filter(a => a >= 0x08000000 && bitmap[(a - 0x08000000) >> 1]).sort((a, b) => a - b);
  return { status: failure?.kind ?? 'pass', failure, phase: progress.phase, frame: session?.frame ?? 0,
    start, end, value, coverage, artifacts: s?.artifacts ?? [], logs, mutations: [...mutations],
    replay: { save: settings.save, time: settings.time, solar: settings.solar, solarLog: session?.solarLog ?? [],
      movie: session?.movie ?? [], ticks, state: lastTick } };
}
