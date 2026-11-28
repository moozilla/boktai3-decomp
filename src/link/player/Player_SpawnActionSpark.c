#include "global.h"
#include "player/player.h"

void SparkParticle_Spawn(void *, u32, const void *, u32, u32, u32, u32, u32);

// confidence=low: chooses a raised position or stored interaction position for the action spark.
void Player_SpawnActionSpark(struct Player *p)
{
    struct SceneVec position;
    struct SceneVec size;

    if (p->actionMode == 3 || p->actionMode == 4) {
        position = p->colliderPosition;
        size.x = 32;
        size.y = 32;
        size.z = 32;
    } else {
        position = p->position;
        position.y += 80;
        size.x = 64;
        size.y = 60;
        size.z = 64;
    }
    SparkParticle_Spawn(&position, 1, &size, 0, 0, 12, 2, 2);
    ++p->particleEmissionCount;
}
