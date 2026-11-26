import { scenario } from '../scenario.mjs';

export default scenario({ empty: true }, function* (s) {
  const { x, y, z } = s.player.position;
  const crate = s.spawn('F04B', { x: x + 160, y, z: z + 160 });
  s.expect(crate.type).equals('struct WoodenCrate');
  s.expect([crate.position.x, crate.position.z]).equals([x + 160, z + 160]);
  const projectile = s.spawn('Scene Projectile', { x, y: y + 64, z, direction: 0, speed: 4, lifetime: 20 });
  const particle = s.spawn('Scene Particle', { screenX: 120, screenY: 80, lifetime: 6 });
  s.expect(projectile.view.params.lifetime).equals(20);
  s.expect(particle.view.lifetime).equals(6);
  const before = projectile.position.x;
  yield s.run(6);
  s.expect(projectile.position.x).above(before);
  s.expect(particle.position.x).above(120);
  yield s.run(40);
  s.expect(projectile.isLive()).equals(false);
  s.expect(particle.isLive()).equals(false);
  return { crate: crate.address, projectile: projectile.address, particle: particle.address };
});
