// Armor_ReceiveColliderContact checks its contact cooldown and guard before
// applying damage. Armor_ReceiveTargetEvent sets flag 0x80 on defeat.
exports.armorCombatant = (addr) => {
  const flags = g.u16(addr + 0xb32), health = g.s16(addr + 0xb1c);
  return {
    boss: 'Armor', health, alive: health > 0 && !(flags & 0x80),
    hitLock: Math.max(0, g.s16(addr + 0xb2c)),
    guard: flags & 2 ? Math.max(0, g.s16(addr + 0xb20)) : 0,
  };
};

