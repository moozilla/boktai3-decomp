#include "global.h"
#include "engine/math/monster_particle_random.h"
#include "scene/collision/packed_contact_vector.h"
#include "bosses/count/scene_graphics_actor.h"
void SparkParticle_Spawn(void *, int, void *, int, int, int, int, int);
void SceneActors_SpawnDestructionParticles(struct SceneGraphicsActor *actor)
{
    struct PackedContactVector size;
    int angle;
    int speed;
    int count;

    size.x = 164;
    size.y = 16;
    size.z = 164;
    angle = (u8)ParticleRandom_ReadDirectSample(gRandomTable);
    count = (ParticleRandom_ReadDirectSample(gRandomTable) & 3) + 2;
    speed = (ParticleRandom_ReadDirectSample(gRandomTable) & 3) + 20;
    SparkParticle_Spawn(&actor->animation.position, 3, &size, angle, count, speed, 2, 2);
    angle = (u8)ParticleRandom_ReadDirectSample(gRandomTable);
    count = (ParticleRandom_ReadDirectSample(gRandomTable) & 3) + 2;
    speed = (ParticleRandom_ReadDirectSample(gRandomTable) & 3) + 20;
    SparkParticle_Spawn(&actor->animation.position, 3, &size, angle, count, speed, 2, 2);
}
