// The key door procedure stores its unlock bit in r708[0x62]. The script
// uses Inventory_HasItemFromScript and consumes a key only after opening.
exports.closedLock = (locks, box) => {
  const addr = g.types.globals.gGameVariables.addr;
  const bank = g.u32(addr);
  if (!bank) return undefined;
  return locks.find((lock) => lock.box === box && !(g.u8(bank + 0x62 + (lock.bit >> 3)) & (1 << (lock.bit & 7))));
};
