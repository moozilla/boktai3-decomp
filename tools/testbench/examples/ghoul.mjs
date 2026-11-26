import { scenario } from '../scenario.mjs';

export default scenario({
  empty: true,
  flags: { 'r708:0x3ed.2:t4': 1 },
  sun: 6,
  time: '14:00',
  equip: { sword: 'Gradius', lens: 'Flame' },
  items: { 'Solar Nut': 3 },
}, function* (s) {
  s.expect(s.where().scene, 'arena scene').equals(2776);
  s.expect(s.variable('r708:0x3c:t1'), 'story').equals(25);
  s.expect(s.variable('r708:0x3ed.2:t4'), 'fixture flag').equals(1);
  s.expect(s.$.gSceneActorRegistryManager.field32, 'ambient registry actors').equals(0);
  s.expect(s.g.fn.SunClock_GetBaseHour(), 'RTC hour').equals(14);
  s.expect(s.g.fn.Solar_GetLevel(), 'initial solar gauge').equals(6);
  s.expect(s.g.fn.Inventory_GetEquipmentId(s.$.gGameState.inventory[s.$.gGameState.unk50]), 'equipped sword').equals(0);
  s.expect(s.$.gGameState.unk828[s.$.gGameState.slots88[4]], 'equipped Flame lens').equals(2);
  s.expect(Array.from({ length: 16 }, (_, i) => s.g.fn.Inventory_GetItemId(i)).filter(id => id === 13).length,
    'Solar Fruit count').equals(3);
  const { x, y, z } = s.player.position;
  const ghoul = s.spawn('Ghoul', { x: x + 95, y, z });
  s.expect(ghoul.type, 'actor type').equals('struct Ghoul');
  const maxHp = ghoul.maxHp;
  s.expect(ghoul.hp, 'initial health').equals(maxHp);
  s.place(s.player, { x: x + 10, z });
  yield s.press('down+right', 4);
  yield s.press('b', 4);
  yield s.run(40);
  s.expect(s.g.fn.Solar_GetLevel(), 'held solar gauge').equals(6);
  const afterInput = ghoul.hp;
  s.damage(ghoul, 9);
  s.expect(ghoul.hp, 'health after damage').below(ghoul.maxHp);
  yield* s.screenshot('ghoul-hit');
  const after = ghoul.hp;
  s.damage(ghoul, 100);
  yield s.run(30);
  return { maxHp, afterInput, after };
});
