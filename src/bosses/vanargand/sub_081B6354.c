#include "global.h"
void ActorStatus_InitializeCollider(void *, s32, s32, s32);
struct Half {
    u8 pad0[0x98];
    u8 collider[0xac];
    u16 w144;
    u16 w146;
    u16 w148;
    u8 pad14a[0xc0];
    u16 w20a;
    u8 pad20c[2];
    u16 w20e;
    u8 pad210[0xc];
};
struct HalfHeading {
    u16 w144;
    u16 w146;
    u16 w148;
};
struct HalfConfig {
    u16 w20a;
    u8 pad20c[2];
    u16 w20e;
    u8 pad210[0x10];
    u16 w220;
};
void PairedSceneActor_InitHalves(struct Half *base, s32 k, s32 a2, s32 a3, s32 s1, s32 s2, s32 s3,
                                 s32 s3b, s32 s4)
{
    u32 down, up;
    s32 i;
    u32 stride;
    u8 *collider;
    struct HalfConfig *config;
    struct HalfHeading *heading;
    stride = sizeof(struct Half);
    i = 0;
    k &= 0x7f;
    up = (0xa0 + k) & 0xff;
    collider = base->collider;
    config = (struct HalfConfig *)&base->w20a;
    heading = (struct HalfHeading *)&base->w144;
    down = (0xa0 - k) & 0xff;
    do {
        if (i == 1)
            base[1].w144 = up;
        else
            heading->w144 = down;
        heading->w146 = a2;
        config->w20a = s1;
        ActorStatus_InitializeCollider(collider, s1, s2, s3);
        config->w220 = s3b;
        config->w20e = a3;
        heading->w148 = s4;
        collider += stride;
        config = (struct HalfConfig *)((u8 *)config + stride);
        heading = (struct HalfHeading *)((u8 *)heading + stride);
        i++;
    } while (i <= 1);
}
