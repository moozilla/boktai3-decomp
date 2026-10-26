#include "global.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Context {
    u8 pad0[0x434];
    struct Vec *target;
    u8 *targetActor;
};
#define U8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define P(p, o)   ((u8 *)(p) + (o))
extern u32 gUnk_02000560;
s32 Math_Atan2(s32, s32);
s32 Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
void Actor_SetAttachmentEffect(void *, s32, s32, s32);
void *TetherActor_GetPosition(void);
static inline s32 Abs(s32 x)
{
    if (x < 0)
        return -x;
    return x;
}
static inline u32 Flag(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline s32 AllowsFacing(u8 *p)
{
    if ((u8)(U8(p, 0x418) - 3) <= 2)
        return 0;
    if (Flag((u32 *)P(p, 0x20), 0x100000))
        return 0;
    if (U16(p, 0x538) && (u16)(U16(p, 0x534) - 5) <= 1)
        return 0;
    {
        u8 mode = U8(p, 0x457);
        u8 allow = 1;
        if (mode > 1 && mode != 7)
            allow = 0;
        return allow;
    }
}
static inline u32 AttachmentFlag(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}
// confidence=low: tracks a target facing and enables contact response when positions and angles agree.
#define SceneActorAnimation_UpdateTargetFacing sub_0802F298

void SceneActorAnimation_UpdateTargetFacing(struct Context *ctx, u8 *p)
{
    struct Vec *pos = (void *)(((u8 *)p) + 0x68);
    s32 facing;
    if (ctx->target) {
        struct Vec *target = ctx->target;
        s32 angle = Math_Atan2(target->x - pos->x, target->z - pos->z);
        if (*(*((u8 **)(((u8 *)p) + 0x2B8)))) {
            register u8 *current asm("r2") = ((u8 *)p) + 0x65;
            s32 diff = (s8)(angle - (*current));
            s32 value;
            register s32 octant asm("r2");
            s32 mask;
            s32 quadrantMask;
            if (diff > 8) {
                value = *current;
                value += 8;
                *current = value;
            } else {
                if (diff < -8) {
                    value = *current;
                    value -= 8;
                } else {
                    value = *current;
                    value += diff;
                }
                *current = value;
                value = (u32)value << 16;
                value = (u32)value >> 16;
            }
            octant = value;
            octant += 112;
            octant >>= 5;
            mask = 7;
            mask &= octant;
            U8(p, 0x1A) = mask;
            value += 96;
            value >>= 6;
            quadrantMask = 3;
            quadrantMask &= value;
            U8(p, 0x19) = quadrantMask;
        }
        facing = ctx->targetActor[0x2D];
    } else
        facing = 0;
    if (*((u8 *)(((u8 *)p) + 9))) {
        *((u8 *)(((u8 *)p) + 9)) = 0;
        *((u8 *)(((u8 *)p) + 7)) = 0;
        Actor_PlayAnimationEntry(p, 0, 1, 0, 4);
        *((u8 *)(((u8 *)p) + 0x1F0)) = 1;
        {
            u8 angle = *((u8 *)(((u8 *)p) + 0x65));
            *((u8 *)(((u8 *)p) + 0x225)) = angle;
        }
        if (((ctx->targetActor && !AllowsFacing(ctx->targetActor)) && (!(gEngineFlags & 0x200))) &&
            (*((u8 *)(((u8 *)p) + 0xB)))) {
            u8 *target = ctx->targetActor;
            u8 tether;
            if ((target[0x457] == 6) && TetherActor_GetPosition())
                tether = 1;
            else
                tether = 0;
            if (tether || AttachmentFlag())
                Actor_SetAttachmentEffect(p, 0, 0, 0);
            else
                Actor_SetAttachmentEffect(p, 3, 0, 0);
        }
    } else
        Actor_PlayAnimationEntry(p, 0, 1, 0, 1);
    {
        u32 mask = 0x1000;
        *((u16 *)(((u8 *)p) + 0xE)) |= mask;
    }
    if (((u8)(*((s32(**)(u8 *))(((u8 *)p) + 0x2E4)))(p)) != 1) {
        struct Vec *target = ctx->target;
        s32 radius = *((u32 *)(((u8 *)p) + 0x228));
        u8 near;
        if (((Abs(target->x - pos->x) < radius) && (Abs(target->y - pos->y) <= 255)) &&
            (Abs(target->z - pos->z) < radius))
            near = 1;
        else
            near = 0;
        if (near) {
            struct Vec *destination = ctx->target;
            s32 direction = (s8)Math_Atan2(pos->x - destination->x, pos->z - destination->z);
            s32 delta = facing - direction;
            s32 oppositeFacing = (*((u8 *)(((u8 *)p) + 0x65))) + 128;
            u8 opposite;
            opposite = facing - oppositeFacing;
            if ((Abs((s8)delta) <= 63) && (Abs((s8)opposite) <= 31)) {
                u32 mask = 0x800;
                *((u16 *)(((u8 *)p) + 0xE)) |= mask;
            }
        }
    }
}
