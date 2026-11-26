import { actorEntry } from './support.mjs';
import { linkFixture, menuScenario } from './menus.mjs';
import { scenario } from './scenario.mjs';

function living(s, actor, id) {
  if (actor.isLive) return actor.isLive();
  if (actor.kind === 'registry') {
    const seen = new Set();
    for (let node = s.$.gSceneActorRegistryNode; node && !seen.has(node.$addr); node = node.next) {
      seen.add(node.$addr);
      if (node.actor?.$addr === actor.address) return actor.base.body.id === id;
    }
    return false;
  }
  if (actor.kind === 'task') return s.g.lib.tasks().some(t => !t.dead && Number(t.task) === actor.address && t.id === id);
  return null;
}

export default function smoke({ actor: key, menu }) {
  if (menu) return menuScenario(menu);
  const entry = actorEntry(key);
  if (entry.stageFixture) return scenario(entry.stageFixture, function* (s) {
    const active = () => s.g.lib.tasks().some(t => !t.dead && t.update === entry.stageUpdate);
    s.expect(active(), `${entry.name} native scene update task`).truthy();
    yield s.run(600);
    s.expect(active(), `${entry.name} survived 600 frames`).truthy();
    yield* s.tap('down');
    return { key, callback: entry.stageUpdate };
  });
  return scenario({ empty: true, equip: { sword: 'Gradius' } }, function* (s) {
    const p = s.player.position;
    const actor = yield* s.atTick(() => {
      if (['30AC', '33FF', '3960'].includes(key)) linkFixture(s);
      return s.spawn(key, { x: p.x + 100, y: p.y, z: p.z });
    });
    s.expect(actor, `${entry.callback} did not yield an observed live allocation`).truthy();
    const callback = actor.kind === 'task' ? s.typed(actor.address, 'struct Task') : null;
    const actorId = actor.kind === 'registry' ? actor.base.body.id : callback?.id;
    const initialHealth = actor.hp ?? null, initialPlayerHealth = s.player.hp;
    yield s.run(600);
    const active = living(s, actor, actorId);
    const beforeAttack = active !== false ? actor.hp ?? null : null;
    for (let attempt = 0; s.player && attempt < 3; attempt++) {
      const player = s.player.position, target = actor.position;
      const dx = (target?.x ?? player.x + 1) - player.x, dz = (target?.z ?? player.z) - player.z;
      const keys = Math.abs(dx) > 2 * Math.abs(dz) ? (dx > 0 ? 'down+right' : 'up+left')
        : Math.abs(dz) > 2 * Math.abs(dx) ? (dz > 0 ? 'down+left' : 'up+right')
        : dx < 0 ? (dz > 0 ? 'left' : 'up') : (dz > 0 ? 'down' : 'right');
      yield s.press(keys, 4);
      yield s.press(keys + '+b', 2);
      yield s.run(22);
      if (living(s, actor, actorId) === false || Number.isInteger(actor.hp) && actor.hp < beforeAttack) break;
    }
    const attacked = living(s, actor, actorId) !== false;
    const afterAttack = attacked ? actor.hp ?? null : null;
    const endPlayerHealth = s.player?.hp ?? null;
    let forced = null;
    if (attacked && Number.isInteger(afterAttack) && afterAttack > 0) {
      s.damage(actor, Math.min(32767, afterAttack));
      forced = actor.hp;
      s.expect(forced, 'health after forced defeat').below(afterAttack);
      yield s.run(60);
    }
    return { key, kind: actor.kind, type: actor.type, callback: callback && s.g.name(callback.update),
      survived600: active, initialHealth, initialPlayerHealth, endPlayerHealth, beforeAttack, afterAttack, forced,
      playerHitActor: beforeAttack !== null && afterAttack !== null && afterAttack < beforeAttack,
      actorHitPlayer: endPlayerHealth !== null && endPlayerHealth < initialPlayerHealth };
  });
}
