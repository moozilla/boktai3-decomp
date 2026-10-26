#include "global.h"
#include "animated_hazard.h"
struct ScriptCombatActor {
    u8 pad0[0x40];
    struct SceneVec position;
    u8 pad48[0x15c - 0x48];
    u32 (*update)(void *);
    u8 animation[0x170 - 0x160];
    s32 attackDistance;
    u8 pad174[0x184 - 0x174];
    u16 timer;
    u8 pad186[8];
    u16 duration;
    s16 enabled;
    u8 pad192[4];
    u16 phase;
    u8 pad198[0x1cc - 0x198];
    void *sequence;
};
u32 ScriptCombatActor_UpdateProjectileAttack(void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
struct CannonSample {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
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
u32 ScriptCombatActor_TryProjectileAttack(struct ScriptCombatActor *p)
{
    struct SceneVec delta;
    struct CannonSample sample;
    s32 i;
    if (p->enabled) {
        if (distanceSquared(Player_GetEffectivePosition(0), &p->position) < p->attackDistance) {
            subtract(&delta, Player_GetEffectivePosition(0), &p->position);
            for (i = 1; i <= 4; i++) {
                s32 index;
                u8 *cell;
                sample.x = p->position.x + Div(delta.x * i, 4);
                sample.y = p->position.y + Div(delta.y * i, 4);
                sample.z = p->position.z + Div(delta.z * i, 4);
                index = cellIndex((struct SceneVec *)&sample);
                cell = cellData(index);
                if (((cell[0] & 15u) << 8) > p->position.y + 128)
                    break;
                if (i == 4) {
                    p->update = ScriptCombatActor_UpdateProjectileAttack;
                    Anim_RestartSequence(p->animation, p->sequence, 1, 0, 0);
                    p->timer = p->duration;
                    p->phase = 0;
                    Sound_PlaySong(0x51f);
                }
            }
        }
    }
    return 0;
}
