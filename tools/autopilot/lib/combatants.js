// Vampire_Create (08029798) installs Vampire_Init as the update in task
// list 8. Vampire_UpdateFrame reads the sprite position at 0x54 and
// Vampire_ApplyPendingDamage keeps signed life at 0x94 and sets flag 2
// on defeat. That handler also tests the script's sunlight flag at 0x3ac.
exports.vampireTargets = () => {
  const out = [];
  const update = g.addr('Vampire_Init') & ~1;
  for (let t = $.gTaskLists[8].head, n = 0; t && n < 512; t = t.next, n++) {
    if ((t.flags & 1) || (t.update & ~1) !== update) continue;
    const addr = t.$addr, flags = g.u32(addr + 0x9c), health = g.s16(addr + 0x94);
    if (health <= 0 || (flags & 2)) continue;
    out.push({
      addr, health, alive: true, boss: 'Vampire',
      x: g.s16(addr + 0x54), y: g.s16(addr + 0x56), z: g.s16(addr + 0x58),
      hidden: Boolean(flags & 0x10),
      solarWeak: Boolean(g.u32(addr + 0x3ac) & 1),
    });
  }
  return out;
};

// Skeleton_UpdateBehaviorCallbacks waits while the actor is hit-locked.
// A rollout can let the lock expire before spending another attack.
exports.combatWait = (target) => target.hidden ? 24 : target.hitLock ? Math.min(16, target.hitLock) : 0;
