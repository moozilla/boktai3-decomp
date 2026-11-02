#include "global.h"
#include "engine_globals.h"
#include "ram.h"
#include "monster_particle_random.h"
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void SparkParticle_Spawn(void *, int, void *, int, int, int, int, int);
void SceneActors_EmitRandomBursts(u8 *p)
{
    u8 *graphics = p + 0x2CC;
    struct Packed size;
    int i;
    for (i = 0; i < 2; i++) {
        int angle, speed;
        int count;
        size.x = 164;
        size.y = 16;
        size.z = 164;
        {
            union RandomTableView table = {gRandomTable};
            angle = (u8)ParticleRandom_ReadDirectSample(table.samples);
            count = (ParticleRandom_ReadDirectSample(table.samples) & 3) + 2;
            speed = (ParticleRandom_ReadDirectSample(table.samples) & 3) + 20;
        }
        SparkParticle_Spawn(graphics + 0x20, 3, &size, angle, count, speed, 2, 2);
    }
}
