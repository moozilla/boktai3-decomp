exports.hresvelgrTargets = () => {
  const globals = g.types.globals, boss = globals.gHresvelgr;
  const addr = boss ? g.u32(boss.addr) : 0;
  if (!addr) return [];
  // Hresvelgr_Create records the actor here. ApplyContactDamage stores its
  // signed life in the travel actor and sets flag 2 when it is defeated.
  const actor = g.at(addr, 'struct HresvelgrTravelActor');
  if (actor.health <= 0 || (actor.flags & 2)) return [];
  const { x, y, z } = actor.position;
  // GetEnvironmentalLevel uses an active area over the actor's position,
  // or an outstanding player effect. Contact handlers gate hits at 0x318.
  const effects = globals.gPlayerEffectPendingCount;
  const solarReactive = Boolean((effects && g.u16(effects.addr)) || g.fn.AreaEffect_IsPositionCovered(addr + 0x54));
  return [{ addr, boss: 'Hresvelgr', health: actor.health, alive: true, x, y, z,
    hitLock: g.u16(addr + 0x318), solarReactive, collisionDisabled: (actor.flags & 0xc) === 0xc }];
};

// GCL procedure 3596 counts hits on the bound Trinity in local byte 0x89
// and starts the loss sequence on the fifth hit.
exports.hresvelgrTrinityHits = () => {
  const localVariables = g.types.globals.gScriptLocalVariables;
  const base = localVariables && g.u32(localVariables.addr);
  return base ? g.u8(base + 0x89) : 0;
};

exports.hresvelgrTrinityCost = (before, after, bossLife) => {
  if (after <= before) return 0;
  if (after >= 5) return 10000;
  return (after - before) * bossLife / (5 - before);
};

// UpdateCollisionPresentation suppresses the attack and body colliders under
// flags 4 and 8. The actor also travels far above normal contact range.
exports.waitForHresvelgr = function* (target) {
  const clearance = 6 * 256;
  const start = exports.where();
  if (!target.collisionDisabled && target.y - start.y <= clearance) return null;
  const session = exports.session, root = session.snapshot(), frame = session.frame;
  const boss = g.at(target.addr, 'struct HresvelgrTravelActor');
  const hits = exports.hresvelgrTrinityHits();
  const safe = () => {
    const now = exports.where();
    return now.scene === start.scene && now.life >= start.life && exports.hresvelgrTrinityHits() === hits;
  };
  const reachable = () => (boss.flags & 0xc) !== 0xc && boss.position.y - exports.where().y <= clearance;
  yield g.until(() => !safe() || exports.message().open || reachable(), 120);
  if (safe()) return { frames: session.frame - frame, trinityHits: hits, bossY: boss.position.y };
  session.restore(root);
  return null;
};
