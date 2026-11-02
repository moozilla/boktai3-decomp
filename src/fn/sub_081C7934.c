#include "scene_actor_terrain.h"
#include "scene_actor_chase_types.h"
struct NpcTag {
    u8 pad[2];
    s16 height;
};
struct Npc {
    u8 pad0[1];
    u8 kind;
    u8 pad2[5];
    u8 done, changed, entered;
    u8 padA[4];
    u16 flags;
    u32 timer;
    u8 pad14[4];
    u8 phase;
    u8 pad19[2];
    u8 angle;
    u8 pad1C[4];
    struct NpcTag tag;
    u8 pad24[0x68 - 0x24];
    struct ContactPosition pos, vel;
    u8 pad78[0x208 - 0x78];
    u32 speed;
};

static inline s32 GetGroundHeight(const struct ContactPosition *pos)
{
    s32 cell = SceneActorTerrain_GetCellIndex(pos);
    struct SceneActorChaseTerrainOverride *override =
        (void *)TerrainModifiers_FindCellOverride(cell, 1);
    struct SceneActorChaseTerrainCell *tile =
        override ? &override->properties
                 : &((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->cells[cell];
    s32 stairs = tile->attributes >> 4;
    s32 height = (tile->attributes & 15) << 8;
    switch (stairs) {
        case 1:
            height -= pos->z & 255;
            break;
        case 2:
            height -= pos->x & 255;
            break;
    }
    return height;
}

void StateActor_UpdateAnimation16State(void *context, struct Npc *p)
{
    u32 r;
    struct ContactPosition *pos;

    if (p->entered != 0) {
        p->entered = 0;
        p->done = 0;
        switch (p->kind) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
                r = 0;
                break;
            case 27:
                r = 0x10;
                break;
            case 28:
            case 29:
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
        {
            u16 mask = 0x80 << 1;
            p->flags |= mask;
        }
    }
    {
        u16 mask = 0x80 << 3;
        p->flags |= mask;
    }

    pos = &p->pos;
    {
        s32 height = GetGroundHeight(pos);
        if (p->pos.y <= height) {
            p->pos.y = height;
            p->done = 1;
        } else {
            s32 v = -p->speed;
            p->vel.y = v;
        }
    }
}
