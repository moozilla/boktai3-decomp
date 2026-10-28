#include "global.h"

struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16;
    u32 pad : 16;
};
extern u8 *gGameState;
extern struct Vec gCameraOrigin;
void Player_SetDirectionalEffectFrame(void *, void *, s32);

static inline s32 Scale(s32 value)
{
    s32 v = value * 3;
    if (v >= 0)
        v >>= 5;
    else
        v = -((-v) >> 5);
    return v;
}

static inline void Add(struct Vec *out, struct Vec *a, struct Vec *b)
{
    out->x = (u16)a->x + (u16)b->x;
    out->y = (u16)a->y + (u16)b->y;
    out->z = (u16)a->z + (u16)b->z;
}

inline static void Project(struct Vec *screen, struct PackedVec *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 depth;
    s32 height;
    s32 y, zscreen;
    screen->x = Scale(x - z);
    depth = Scale(x + z);
    height = Scale(world->y);
    y = depth - height;
    zscreen = depth + height;
    screen->x = (((u16)screen->x) - ((u16)gCameraOrigin.x)) + 120;
    screen->y = ((y) - ((u16)gCameraOrigin.y)) + 80;
    screen->z = (zscreen) - ((u16)gCameraOrigin.z);
}

inline static struct Vec *SetVec(struct Vec *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
    return v;
}

struct PlayerEffectPair {
    u8 pad0[0x2D];
    u8 direction;
    u8 pad2E[2];
    struct Vec position;
    u8 pad38[0x774 - 0x38];
    void *frames;
    u32 firstFlags;
    u8 pad77C[0x790 - 0x77C];
    struct Vec firstPosition;
    u8 pad798[8];
    u32 secondFlags;
    u8 pad7A4[0x7B8 - 0x7A4];
    struct Vec secondPosition;
    u8 pad7C0[8];
    u8 pending;
};

void sub_0813BAD8(struct PlayerEffectPair *p)
{
    struct Vec offsets[2];
    struct PackedVec position;
    struct Vec projected;
    s32 direction;
    if (p->pending != 1) {
        p->firstFlags |= 1;
        p->secondFlags |= 1;
        return;
    }
    p->pending = 0;
    p->firstFlags &= ~1;
    p->secondFlags &= ~1;
    direction = ((p->direction + 0x70) >> 5) & 7;
    switch (direction) {
        case 1:
            Player_SetDirectionalEffectFrame(&p->firstFlags, p->frames,
                                             15 - (*((s16 *)(gGameState + 0x10))));
            Player_SetDirectionalEffectFrame(&p->secondFlags, p->frames,
                                             13 - (*((s16 *)(gGameState + 0x10))));
            SetVec(&offsets[0], -28, -44, 0);
            SetVec(&offsets[1], 12, -4, 0);
            break;

        case 3:
            Player_SetDirectionalEffectFrame(&p->firstFlags, p->frames,
                                             9 - (*((s16 *)(gGameState + 0x10))));
            Player_SetDirectionalEffectFrame(&p->secondFlags, p->frames,
                                             15 - (*((s16 *)(gGameState + 0x10))));
            SetVec(&offsets[0], 16, -32, 0);
            SetVec(&offsets[1], -24, 8, 0);
            break;

        case 5:
            Player_SetDirectionalEffectFrame(&p->firstFlags, p->frames,
                                             11 - (*((s16 *)(gGameState + 0x10))));
            Player_SetDirectionalEffectFrame(&p->secondFlags, p->frames,
                                             9 - (*((s16 *)(gGameState + 0x10))));
            SetVec(&offsets[0], 16, 8, 0);
            SetVec(&offsets[1], -24, -32, 0);
            break;

        case 7:
            Player_SetDirectionalEffectFrame(&p->firstFlags, p->frames,
                                             13 - (*((s16 *)(gGameState + 0x10))));
            Player_SetDirectionalEffectFrame(&p->secondFlags, p->frames,
                                             11 - (*((s16 *)(gGameState + 0x10))));
            SetVec(&offsets[0], -20, -4, 0);
            SetVec(&offsets[1], 20, -44, 0);
            break;

        default:
            p->firstFlags |= 1;
            p->secondFlags |= 1;
            return;
    }

    do {
        position.x = p->position.x;
    } while (0);
    position.z = p->position.z;
    position.y = p->position.y;
    Project(&projected, &position);
    offsets[0].x -= 4;
    offsets[0].y -= 4;
    offsets[1].x -= 4;
    offsets[1].y -= 4;
    Add(&p->firstPosition, &projected, &offsets[0]);
    Add(&p->secondPosition, &projected, &offsets[1]);
    return;
}
