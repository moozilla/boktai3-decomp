// PileDriver_WaitInitialTether takes the coffin when its body reaches the
// panel's center. PileDriver_UpdateActiveSubjects takes a charging player
// at the approach position, after gunfire has activated the generators.
exports.purificationPanel = () => $.gPileDriver?.initialMode === 0 && !!$.gTetherActor;

exports.preparePurification = function* (opts = {}) {
  const panel = $.gPileDriver;
  const scene = exports.where().scene, story = exports.story();
  const changed = () => exports.where().scene !== scene || exports.story() > story;
  const trace = opts.trace || (() => {});
  const subjects = () => Array.from(panel.subjects).slice(0, panel.subjectCount).filter(Boolean);
  const interrupted = () => changed() || exports.message().open;
  const scriptControlsPlayer = () => Boolean(g.fn.ScriptUtility_IsControlFlagSet());

  for (let step = 0; step < 24 && !changed(); step++) {
    if (scriptControlsPlayer()) {
      yield g.until(() => interrupted() || !scriptControlsPlayer(), 180);
      if (!changed()) yield* exports.clearInterruptions({ ...opts });
      continue;
    }
    const phase = panel.effectState;
    trace({ phase, subjects: subjects().map((s) => s.state) });
    if (phase === 0) {
      if (!exports.coffinAvailable()) return { done: false, reason: 'no coffin to carry' };
      const { x, z } = panel.center, coffin = $.gTetherActor.position, p = $.gPlayers[0].position;
      // Pull the trailing coffin across the center. Leave enough room for
      // the acquisition to trigger before releasing A to read the script.
      const length = Math.hypot(x - coffin.x, z - coffin.z) || 1;
      const trail = Math.max(80, Math.hypot(p.x - coffin.x, p.z - coffin.z));
      const target = [x + Math.round((x - coffin.x) * trail / length), z + Math.round((z - coffin.z) * trail / length)];
      const result = yield* exports.carryCoffin(target, { ...opts, stopIf: () => changed() || panel.effectState !== 0 });
      trace({ dock: target, docked: !changed() && panel.effectState !== 0, ...result });
      if (!changed() && panel.effectState === 0) return { done: false, reason: result.reason };
    } else if (phase === 3 || phase === 4) {
      const waiting = subjects().filter((s) => s.state === 2);
      if (waiting.length) {
        // Pile Driver instruction 9265. The generator advances on a contact
        // with the attack mask in PileDriverSubject_ApplyContact.
        const weapon = yield* exports.quickChange({ gun: true });
        trace({ weapon });
        for (const subject of waiting) {
          if (changed()) break;
          const { x, z } = subject.home;
          const result = yield* exports.hitAt(x >> 8, z >> 8, { ...opts, hits: 4, tolerance: 24,
            done: () => changed() || subject.state > 2 });
          trace({ subject: [x, z], state: subject.state, ...result });
          if (!result.done) return { done: false, reason: 'subject did not respond to attack' };
        }
      } else if (phase === 4 && subjects().every((s) => s.state === 4)) {
        // Text 9278–9279 and PileDriver_AdvancePlayerArrival require sunlight
        // at the approach point. The sensor's smaller count is brighter.
        exports.session.setSolar(64);
        yield g.until(() => interrupted() || exports.solarStatus().level > 0, 180);
        trace({ sunlight: exports.solarStatus() });
        if (interrupted()) {
          if (!changed()) yield* exports.clearInterruptions({ ...opts });
          continue;
        }
        if (exports.solarStatus().level === 0) return { done: false, reason: 'panel has no effective sunlight' };
        const { x, z } = panel.approachPosition;
        const result = yield* exports.navigate([x, z], { ...opts, tolerance: 32, stopIf: changed });
        if (!result.reached && !changed()) return { done: false, reason: result.reason };
        if (!changed()) yield g.press('a', 8);
        trace({ charge: [x, z], state: panel.effectState });
        if (!changed() && panel.effectState === phase) return { done: false, reason: 'charge did not engage panel' };
      } else {
        const settled = () => phase === 4 && subjects().every((s) => s.state === 4);
        yield g.until(() => interrupted() || panel.effectState !== phase || settled(), 180);
        if (!interrupted() && panel.effectState === phase && !settled())
          return { done: false, reason: 'subjects did not settle' };
      }
    } else {
      yield g.until(() => interrupted() || panel.effectState !== phase, 600);
      if (!interrupted() && panel.effectState === phase) return { done: false, reason: 'panel phase did not advance' };
    }
    if (!changed()) yield* exports.clearInterruptions({ ...opts });
  }
  return changed() ? { done: true } : { done: false, reason: 'panel step limit' };
};
