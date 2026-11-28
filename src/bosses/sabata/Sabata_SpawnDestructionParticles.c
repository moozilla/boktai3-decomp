#include "global.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"
#include "scene/collision/contact_position.h"
/* Sabata_SpawnDestructionParticles: same as SceneActors_SpawnDestructionParticles but spawns the sparks on the object itself (provisional name) */
void SparkParticle_Spawn(void *, int, void *, int, int, int, int, int);
void Sabata_SpawnDestructionParticles(u8 *p)
{
    struct ContactPosition size;
    int angle;
    int speed;
    int count;
    size.x = 164;
    size.y = 16;
    size.z = 164;
    angle = (u8)ParticleRandom_ReadDirectSample(gRandomTable);
    count = (ParticleRandom_ReadDirectSample(gRandomTable) & 3) + 2;
    speed = (ParticleRandom_ReadDirectSample(gRandomTable) & 3) + 20;
    SparkParticle_Spawn(p, 3, &size, angle, count, speed, 2, 2);
    angle = (u8)ParticleRandom_ReadDirectSample(gRandomTable);
    count = (ParticleRandom_ReadDirectSample(gRandomTable) & 3) + 2;
    speed = (ParticleRandom_ReadDirectSample(gRandomTable) & 3) + 20;
    SparkParticle_Spawn(p, 3, &size, angle, count, speed, 2, 2);
}
