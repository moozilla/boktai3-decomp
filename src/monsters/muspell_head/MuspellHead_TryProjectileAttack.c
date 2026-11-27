#include "monsters/muspell_head.h"
#include "objects/hazards/animated_hazard.h"
#include "recovered/contact_sample_vector_layout.h"
u32 MuspellHead_UpdateProjectileAttack(void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
s32 Div(s32, s32);
static inline void subtract(struct SceneVec *v, struct SceneVec *a, struct SceneVec *b)
{
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}
static inline s32 cellIndex(struct SceneVec *v)
{
    s32 x = v->x >> 8, z = v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *cellData(s32 index)
{
    u8 *override = TerrainModifiers_FindCellOverride(index, 1);
    if (override)
        return override + 4;
    return index * 4 + 12 + gTerrainMap->cells;
}
static inline s32 distanceSquared(struct SceneVec *a, struct SceneVec *b)
{
    s32 x = a->x - b->x;
    s32 z = a->z - b->z;
    return x * x + z * z;
}
u32 MuspellHead_TryProjectileAttack(struct MuspellHead *p)
{
    struct SceneVec delta;
    struct ContactSampleVector sample;
    s32 i;
    if (p->enabled) {
        if (distanceSquared(Player_GetEffectivePosition(0), (struct SceneVec *)&p->sprite.position) < p->attackDistance) {
            subtract(&delta, Player_GetEffectivePosition(0), (struct SceneVec *)&p->sprite.position);
            for (i = 1; i <= 4; i++) {
                s32 index;
                u8 *cell;
                sample.x = p->sprite.position.x + Div(delta.x * i, 4);
                sample.y = p->sprite.position.y + Div(delta.y * i, 4);
                sample.z = p->sprite.position.z + Div(delta.z * i, 4);
                index = cellIndex((struct SceneVec *)&sample);
                cell = cellData(index);
                if (((cell[0] & 15u) << 8) > p->sprite.position.y + 128)
                    break;
                if (i == 4) {
                    p->action = (void (*)(void))MuspellHead_UpdateProjectileAttack;
                    Anim_RestartSequence(&p->playback, p->sequence, 1, 0, 0);
                    p->timer = p->duration;
                    p->phase = 0;
                    Sound_PlaySong(0x51f);
                }
            }
        }
    }
    return 0;
}
