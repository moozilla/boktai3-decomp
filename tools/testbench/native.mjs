// Native pool fixtures for creators that are called from C and have no GCL key.
export const nativeActors = {
  'native:scene-particle': {
    spawn(s, opts) {
      const position = s.allocate('struct SceneParticleVector', { x: opts.screenX ?? 120, y: opts.screenY ?? 80 });
      const velocity = s.allocate('struct SceneParticleVector', { x: opts.velocityX ?? 2, y: opts.velocityY ?? -1 });
      const address = s.call('SceneParticle_Create', position, velocity, opts.lifetime ?? 30, opts.kind ?? 0, opts.mode ?? 1) >>> 0;
      if (!address) return null;
      const pool = s.$.gSceneParticlePool;
      const view = s.typed(address, 'struct SceneParticle');
      const slot = view.slotIndex;
      return s.actor(view, 'slot', { position: 'position', isLive: () => !!(pool.occupied & (1 << slot)) });
    },
  },
  'native:scene-projectile': {
    spawn(s, opts) {
      const player = s.player.position;
      const position = s.allocate('struct OrbitEffectPosition', { x: opts.x ?? player.x, y: opts.y ?? player.y, z: opts.z ?? player.z });
      const parameters = s.allocate('struct SceneProjectileParams');
      s.call('Projectile_InitParameters', parameters);
      s.call('Projectile_SetLaunchParameters', parameters, opts.movement ?? 0, opts.lifetime ?? 60, opts.direction ?? 0, opts.speed ?? 8);
      const address = s.$.gSceneProjectilePool || s.call('SceneProjectile_CreatePool') >>> 0;
      if (!address) return null;
      const pool = s.typed(address, 'struct SceneProjectilePool');
      const index = Array.from({ length: pool.slots.length }, (_, i) => i).find(i => !(pool.used & (1 << i)));
      if (index === undefined || s.call('Projectile_SpawnAtPosition', parameters, position) < 0) return null;
      return s.actor(pool.slots[index], 'slot', { position: 'sprite.position', isLive: () => !!(pool.used & (1 << index)) });
    },
  },
};
