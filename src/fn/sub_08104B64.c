/* Provisional ActorGroup names: static behavior established; semantic ownership tentative. */
#include "global.h"
#include "engine.h"
#include "scene_vec.h"
struct ActorGroupMember {
    u8 unk0[0x1C];
    struct SceneVec position;
    u8 unk24[0x100 - 0x24];
    u8 unk100[0x20];
    s16 motionX, velocity, motionZ;
    u8 unk126[0x22];
    u16 ground;
    u8 unk14A[8];
    u16 state;
    u8 unk154[10];
    u8 started;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void SpriteParticle_CreateDefaultVariant4384(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                             u32, u32);
static inline s32 cellIndex(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 groundHeight(struct SceneVec *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    struct SceneVec *savedPosition = position;
    s32 index, height;
    u8 *cell;
    u32 kind;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)savedPosition->z;
            break;
        case 2:
            height -= (u8)savedPosition->x;
            break;
    }
    return height;
}
static inline void ClearMotion(void *dest)
{
    u32 zero = 0;
    CpuSet(&zero, dest, 0x05000002);
}
void ActorGroup_UpdateMemberFall(void *unused, struct ActorGroupMember *member)
{
    s32 i;
    if (member->started) {
        member->ground = groundHeight(&member->position);
        member->started = 0;
        ClearMotion(&member->motionX);
    } else {
        member->position.y -= member->velocity;
        member->velocity += 2;
        if (member->ground >= member->position.y) {
            member->position.y = member->ground;
            member->state = 4;
            member->started = 1;
            for (i = 0; i < 8; i++)
                SpriteParticle_CreateDefaultVariant4384(&member->position, 8, 16, 4, 12, 18, i << 5,
                                                        8, 64, 1, 1, 0);
        }
    }
}
