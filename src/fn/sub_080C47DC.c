#include "global.h"
// Golem settle step, called with the actor context.
#include "engine.h"
#include "contact_actor.h"

struct Unk080C47DCObject {
    u8 unknown0[0x1C];
    struct ContactPosition position;
};
struct Unk080C47DCBody {
    u8 unknown0[0xC];
    struct ContactPosition position;
};
struct Unk080C47DCDelta {
    s16 x, y, z, pad;
};
struct Unk080C47DCLink {
    u8 unknown0[0x1FC];
    struct Unk080C47DCObject *target;
};

s32 Terrain_ApplyBlocked(void *, void *, void *);

static inline u32 Cell(s32 x, s32 z)
{
    if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) || (((u32)z) >= gTerrainHeight))
        return 0;
    return gTerrainMap->rows[z] + x;
}

s32 sub_080C47DC(u8 *actor)
{
    u8 *owner = actor;
    struct Unk080C47DCLink *link = (struct Unk080C47DCLink *)(actor + 0x3D4);
    struct Unk080C47DCBody *body = (struct Unk080C47DCBody *)(actor + 0x50);
    struct Unk080C47DCObject **target = (struct Unk080C47DCObject **)(actor + 0x5D0);
    s32 found = Terrain_ApplyBlocked(*target, actor + 0x5C, actor + 0x69C);
    if (found != 0) {
        s32 dx = (*target)->position.x - body->position.x;
        s32 dz = (*target)->position.z - body->position.z;
        u8 *direction;
        struct Unk080C47DCDelta delta;
        struct Unk080C47DCObject **goalTarget;
        if (Cell((*target)->position.x >> 8, (*target)->position.z >> 8) !=
            Cell(body->position.x >> 8, body->position.z >> 8)) {
            if (((dx < 0) ? (-dx) : (dx)) < ((dz < 0) ? (-dz) : (dz)))
                link->target->position.x = (body->position.x & (~0xFF)) + 0x80;
            else {
                struct ContactPosition *position = &body->position;
                link->target->position.z = (position->z & (~0xFF)) + 0x80;
            }
        }
        goalTarget = &link->target;
        dx = (*goalTarget)->position.x - body->position.x;
        dz = (*goalTarget)->position.z - body->position.z;
        {
            u8 angle = Math_Atan2(dx, dz);
            direction = actor + 0x68E;
            *direction = angle;
        }
        *direction = ((((*direction) + 0x20) & 0xFF) >> 6) << 6;
        {
            struct Unk080C47DCDelta *out = &delta;
            struct ContactPosition *from = (struct ContactPosition *)(owner + 0x5C);
            struct ContactPosition *goal = &(*goalTarget)->position;
            s32 quadrant;
            {
                struct Unk080C47DCDelta *first = &delta;
                first->x = ((u16)goal->x) - ((u16)from->x);
            }
            out->y = ((u16)goal->y) - ((u16)from->y);
            out->z = ((u16)goal->z) - ((u16)from->z);
            quadrant = Math_Atan2(delta.x, out->z);
            quadrant = ((quadrant + 0x20) & 0xFF) >> 6;
            if ((delta.x != (-out->z)) && (quadrant != ((*direction) >> 6)))
                Memory_Clear(owner + 0x64, 8);
        }
    } else {
        *((u16 *)(actor + 0x680)) = found;
    }
    return 1;
}
