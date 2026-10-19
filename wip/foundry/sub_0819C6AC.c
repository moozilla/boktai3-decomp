#include "global.h"
#define ColliderActor_RandomizeTargetPosition sub_0819C6AC
#define gRandomTable                          gUnk_0203B400
#define gRandomIndex                          gUnk_03005308
#define gSineTable                            gUnk_086149C4
struct ColliderActorVector {
    s16 x, y, z, pad;
};
struct ColliderActor {
    u8 pad0[0x110];
    struct ColliderActorVector *cameraAnchor;
    u8 pad114[0x32b - 0x114];
    u8 direction;
    u8 pad32C[0x39c - 0x32c];
    struct ColliderActorVector target;
};
extern u32 gRandomIndex;
extern const s16 gSineTable[];
extern const u16 gRandomTable[];
static inline s32 RandomRadius(s32 random)
{
    return (random >> 3) % 512 + 256;
}
static inline s32 RandomAngle(s32 random)
{
    return (random >> 3) % 32 + 112;
}
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
void sub_0819C6AC(struct ColliderActor *actor)
{
    {
        struct ColliderActor *anchorOwner = actor;
        s32 radius, angle, value, random;
        u32 index, next;
        struct ColliderActorVector *anchor, *out;
        const u16 *table = gRandomTable;
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        random = *(const u16 *)((index << 1) + (u32)table);
        radius = RandomRadius(random);
        next = (index + 1) & 1023;
        gRandomIndex = next;
        random = *(const u16 *)((next << 1) + (u32)table);
        value = actor->direction << 6;
        angle = value + RandomAngle(random);
        angle &= 255;
        out = &actor->target;
        out->x = Scale(gSineTable[(angle + 64) & 255] * radius);
        out->y = 0;
        out->z = Scale(gSineTable[angle & 255] * radius);
        anchor = anchorOwner->cameraAnchor;
        out = &actor->target;
        out->x += anchor->x;
        out->y += anchor->y;
        out->z = out->z + anchor->z;
        {
            s16 *dest = &actor->target.y;
            s16 height = 832;
            *dest = height;
        }
    }
}
