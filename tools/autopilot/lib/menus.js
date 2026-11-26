// Menu skills for the autopilot (an agent library script: `g`, `$`, `print` in
// scope, skills go on `exports`; see port/agent/README.md "Library").
//
// Every menu is a scheduler task whose struct starts with the task header, so the
// task's address is a typed view of the menu. Each entry below reads that view and
// follows the menu's own update function in src/ to decide the next key; no fixed
// input sequences. `yield* menuStep(policy)` handles whichever known menu is active
// and returns what it did; `activeMenu()` names it.
//
// Policy (all optional): { password: 'no' | 'yes', region: index, name: null }.
// name null keeps the game's default name (an empty name falls back to it).

const KEY = { a: 1, b: 2, select: 4, start: 8, right: 16, left: 32, up: 64, down: 128, r: 256, l: 512 };

// update function -> { struct, ready(view): accepts input now, decide(view, policy): key name }
const MENUS = {
  // src/solar_calibration_menu.c: state 0 confirms with A once timer > 30.
  SolarCalibration_Update: {
    struct: 'struct SolarCalibrationMenu',
    ready: (v) => v.state === 0 && v.timer > 30,
    decide: () => 'a',
  },
  // src/date_entry_menu.c: state 0; START sets the clock to the shown date.
  DateMenu_Update: {
    struct: 'struct DateMenu',
    ready: (v) => v.state === 0,
    decide: () => 'start',
  },
  // src/region_menu.c: state 0; A picks the highlighted region (up/down move it).
  Time_HandleRegionKeypad: {
    struct: 'struct RegionMenu',
    ready: (v) => v.state === 0,
    decide: (v, p) => (p.region === undefined || p.region === v.selected ? 'a' : p.region < v.selected ? 'up' : 'down'),
  },
  // src/timed_presentation.c (title logo, notices): state 0; A skips once
  // timer > 1 << minShift (and the heap guard holds); otherwise it times out.
  TimedPresentation_Update: {
    struct: 'struct TimedPresentation',
    ready: (v) => v.state === 0 && v.timer > (1 << v.minShift) + 1 && $.gHeapEndGuard === 0x516B92EE,
    decide: () => 'a',
  },
  // src/fn/sub_081EAB64.c, the title screen. Its struct Display is local to that
  // file (not in types.json), so `view` is a raw-offset reader of the same layout.
  // States 1 and 5 skip the logo animation with A (after 10 frames / once), state 6
  // is PRESS START (A or START once timer > 1 << fadeShift), state 8 the menu
  // (left/right, A or START picks selection; 0 = start the game).
  Params3D0_UpdateDisplay: {
    struct: null,
    view: (a) => ({ get selection() { return g.u32(a + 0x724) | 0; }, get count() { return g.u32(a + 0x728) | 0; },
      get state() { return g.u32(a + 0x72c); }, get fadeShift() { return g.u32(a + 0x738) | 0; },
      get stateTimer() { return g.u32(a + 0x748) | 0; }, get timer() { return g.u32(a + 0x754) | 0; },
      get skipped() { return g.u32(a + 0x764) | 0; } }),
    ready: (v) => (v.state === 1 && v.stateTimer > 10) || (v.state === 5 && !v.skipped) ||
      ((v.state === 6 || v.state === 8) && v.timer >= (1 << v.fadeShift) + 1),
    decide: (v, p) => {
      if (v.state !== 8) return 'a';
      const want = p.title ?? 0;
      return v.selection === want ? 'a' : 'right';
    },
  },
  // src/game_start.c: state 0; selection 0 = continue the save, 1 = start from the
  // beginning; A runs it. Up/down only move when gGameState->flag86E (a save exists).
  GameStart_Update: {
    struct: 'struct GameStart',
    ready: (v) => v.state === 0,
    decide: (v, p) => {
      const want = (p.gameStart ?? 'new') === 'continue' ? 0 : 1;
      const canMove = (g.s16($.gGameState.$addr + 0x86e)) !== 0;
      return v.selection === want || !canMove ? 'a' : 'down';
    },
  },
  // src/screen_message.c: state 0; with choices, selection 0 = yes, 1 = no;
  // left/right toggle, A answers. The one before name entry asks for a Boktai 2 password.
  ScreenMessage_Update: {
    struct: 'struct ScreenMessage',
    ready: (v) => v.state === 0,
    decide: (v, p) => {
      if (!$.gScreenParams.showChoices) return 'a';
      const want = (p.password ?? 'no') === 'yes' ? 0 : 1;
      return v.selection === want ? 'a' : want === 0 ? 'left' : 'right';
    },
  },
  // src/name_entry.c: state 0; START jumps the cursor to the end key (0x56), A or
  // START there confirms; a blank name becomes the default.
  NameEntry_Update: {
    struct: 'struct NameEntryTask',
    ready: (v) => v.state === 0,
    decide: (v) => (v.selection === 0x56 ? 'a' : 'start'),
  },
};
exports.MENUS = MENUS;

// The known menu whose task is alive: { name, task, view } or null.
exports.activeMenu = () => {
  for (const t of exports.tasks()) {
    if (t.dead) continue;
    const spec = t.update === 'GameOver_OnUpdate' ? exports.gameOverMenu : MENUS[t.update];
    if (!spec) continue;
    const task = Number(t.task), view = spec.struct ? g.at(task, spec.struct) : spec.view(task);
    if (spec.active && !spec.active(view)) continue;
    return { name: t.update, task, id: t.id, view, spec };
  }
  return null;
};

function alive(m) {
  return exports.tasks().some((t) => !t.dead && Number(t.task) === m.task && t.id === m.id && t.update === m.name) &&
    (!m.spec.active || m.spec.active(m.view));
}

// Drive the active menu until its task ends. Presses one key for one frame, then
// releases (the menus read new presses, gPadState[1]). A key goes in only after the
// menu has been ready for `slack` frames. Each key records `calls`: how many times
// the menu's update function had run for this task, which is what a replay on the
// original ROM syncs to (hardware has lag frames in which the update does not run;
// the port has none). Returns { menu, update (ROM address), task, keys: [{key, at,
// calls}], frames, done }, or null when no known menu is active.
exports.menuStep = function* (policy = {}, max = 1800, slack = 2) {
  const m = exports.activeMenu();
  if (!m) return null;
  const spec = m.spec, start = g.frameNo, keys = [];
  let calls = 0, readyFor = 0;
  const hook = g.on(m.name, (args) => { if ((args[0] >>> 0) === m.task) calls++; });
  try {
    while (alive(m) && g.frameNo - start < max) {
      if (spec.ready(m.view)) {
        if (++readyFor > slack) {
          const k = spec.decide(m.view, policy);
          keys.push({ key: k, at: g.frameNo - start, calls });
          yield g.press(KEY[k], 1);
          readyFor = 0;
        }
      } else readyFor = 0;
      yield g.press(0, 1);
    }
  } finally { g.off(hook); }
  return { menu: m.name, update: g.addr(m.name) & ~1, task: m.task, keys, frames: g.frameNo - start, done: !alive(m) };
};
