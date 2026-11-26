// The shop's Shop_TickController invokes the active callback in
// SceneInventoryLifecycleView.action. Its purchase phase reads category and
// the saved grid selection (Shop_TickPurchaseTransition, 08062414).
exports.shopState = () => {
  const t = exports.tasks().find((task) => !task.dead && task.update === 'Shop_TickController');
  if (!t) return null;
  const address = Number(t.task);
  const panel = g.at(address, 'struct SceneInventoryTail');
  const action = g.u32(address + 0x1cf8);
  const grid = panel.grid;
  return { address, callback: g.name(action), mode: panel.mode, category: panel.category,
    page: panel.page, selection: grid.selection, savedSelection: grid.savedSelection, choice: panel.choiceState };
};

const healing = (items) => items.filter((id) => id === 0 || id === 1).length;
exports.buyHealing = function* ({ max = 180 } = {}) {
  const session = exports.session;
  const before = healing(exports.bags().items), funds = $.gGameState.unk7B0;
  if (!funds || before >= 2 || !exports.shopState()) return { done: false, before };
  const root = session.snapshot(), seen = new Set(), queue = [[]];
  let found, attempts = 0;
  const play = function* (key) {
    yield g.press(key, 2);
    yield g.press(0, key === 'a' || key === 'b' ? 40 : 14);
  };
  search: for (let i = 0; i < queue.length && attempts < max; i++) {
    const path = queue[i];
    for (const key of ['a', 'down', 'up', 'right', 'left', 'r', 'l', 'b']) {
      if (++attempts > max) break search;
      session.restore(root);
      for (const step of path) yield* play(step);
      yield* play(key);
      const now = exports.shopState();
      if (!now) continue;
      if (now.callback === 'Shop_TickPurchaseTransition') yield g.press(0, 80);
      if (healing(exports.bags().items) > before && $.gGameState.unk7B0 < funds) {
        found = [...path, key];
        break search;
      }
      const state = JSON.stringify(exports.shopState());
      if (!seen.has(state)) {
        seen.add(state);
        queue.push([...path, key]);
      }
    }
  }
  if (!found) session.restore(root);
  return { done: Boolean(found), keys: found, before, after: healing(exports.bags().items),
    spent: funds - $.gGameState.unk7B0, attempts: Math.min(attempts, max) };
};

exports.leaveShop = function* () {
  for (let i = 0; i < 8 && exports.shopState(); i++) {
    yield g.press('b', 2);
    yield g.press(0, 40);
  }
};
