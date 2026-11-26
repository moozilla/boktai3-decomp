import { scenario } from './scenario.mjs';

export const pausePages = ['map', 'items', 'valuables', 'solar gun', 'sword', 'accessory', 'status', 'config', 'sleep', 'save'];
const menus = {
  'weapon shop': { key: '744F', update: 'SceneInventory_TickController' },
  'item shop': { key: '3019', update: 'SceneInventory_TickController' },
  'accessory shop': { key: '7300', update: 'SceneInventory_TickController' },
  'link lobby': { key: '33FF', update: 'LinkLobbyPanel_OnTaskUpdate', link: true },
  'link save': { key: '3960', update: 'SaveMenu_Update', link: true },
  'monster book': { key: '34EA', update: 'MonsterBook_UpdateNavigation' },
  'forging book': { key: '7CA2', update: 'sub_0821290C' },
  'collection book': { key: 'A2DB', update: 'sub_08213888' },
};
export const menuFixtures = [...pausePages.map(name => `pause/${name}`), ...Object.keys(menus)];

function hud(s) {
  const task = s.g.lib.tasks().find(t => !t.dead && t.update === 'Hud_UpdateWithCallback');
  return task && s.typed(Number(task.task), 'struct SceneHudState');
}

function linkFixture(s) {
  // The local record/save screens need an active session and a selected record.
  // A zeroed peer count exercises the display without wireless negotiation.
  s.$.gLinkSession = s.allocate('struct AreaEffectLinkState', {
    unknown1C: 1, unavailable: 1, outgoingChoices: { 0: 5 }, participantCount: 0, localPlayer: 0,
    menuCallback: s.g.addr('Video_OnFrameNoOp'),
  });
}

export function* openMenu(s, name) {
  if (name.startsWith('pause/')) {
    const page = pausePages.indexOf(name.slice('pause/'.length));
    if (page < 0) throw new Error(`Unknown pause page ${name}`);
    const isOpen = () => hud(s)?.updateCallback === s.g.addr('SceneHud_UpdatePageInput');
    if (!isOpen()) yield* s.tap('start', 40);
    yield* s.waitFor(isOpen, 90, 'pause menu');
    for (let i = 0; i < pausePages.length && hud(s).mode !== page; i++) {
      const rightSteps = (page - hud(s).mode + pausePages.length) % pausePages.length;
      yield* s.tap(rightSteps <= pausePages.length / 2 ? 'r' : 'l', 16);
    }
    s.expect(hud(s).mode, 'pause page id').equals(page);
    return { name, view: hud(s), callback: s.g.name(hud(s).pageCallback) };
  }
  const fixture = menus[name];
  if (!fixture) throw new Error(`Unknown menu ${name}`);
  if (fixture.link) linkFixture(s);
  const actor = s.spawn(fixture.key);
  s.expect(actor, `${name} creator returned a live object`).truthy();
  yield s.run(10);
  const task = s.g.lib.tasks().find(t => !t.dead && t.update === fixture.update);
  s.expect(task, `${name} update task`).truthy();
  return { name, view: actor.view, callback: fixture.update };
}

export function menuScenario(name, { screenshot = false } = {}) {
  // The committed Continue fixture enables all pause tabs. The arena has the
  // scene managers that the independently created native panels expect.
  const field = name.startsWith('pause/') ? {} : { empty: true };
  return scenario(field, function* (s) {
    const menu = yield* openMenu(s, name);
    if (screenshot) yield* s.screenshot(name.replaceAll(/[^a-z0-9]+/gi, '-'));
    yield* s.tap('down', 10);
    yield* s.tap('right', 12);
    return { name, callback: menu.callback };
  });
}
