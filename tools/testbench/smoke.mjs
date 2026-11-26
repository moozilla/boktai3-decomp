import { actorEntry } from './support.mjs';
import { menuScenario } from './menus.mjs';
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
  return scenario({ empty: true }, function* (s) {
    const p = s.player.position;
    const actor = s.spawn(key, { x: p.x + 100, y: p.y, z: p.z });
    s.expect(actor, `${entry.callback} did not yield an observed live allocation`).truthy();
    const callback = actor.kind === 'task' ? s.typed(actor.address, 'struct Task') : null;
    const actorId = actor.kind === 'registry' ? actor.base.body.id : callback?.id;
    const initialHealth = actor.hp ?? null, initialPlayerHealth = s.player.hp;
    yield s.run(600);
    const active = living(s, actor, actorId);
    const beforeAttack = active !== false ? actor.hp ?? null : null;
    if (s.player) {
      yield s.press('down+right', 4);
      yield s.press('b', 4);
      yield s.run(24);
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
