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
  const solarReactive = Boolean((effects && g.u32(effects.addr)) || g.fn.AreaEffect_IsPositionCovered(addr + 0x54));
  return [{ addr, boss: 'Hresvelgr', health: actor.health, alive: true, x, y, z,
    hitLock: g.u16(addr + 0x318), solarReactive }];
};
