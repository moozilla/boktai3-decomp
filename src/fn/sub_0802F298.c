#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
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
#define U32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define P(p, o)   ((u8 *)(p) + (o))
#define PTR(p, o) (*(u8 **)P(p, o))
extern u32 gUnk_030053F4;
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
static inline s32 Blocked(u8 *p)
{
    if ((u8)(U8(p, 0x418) - 3) <= 2)
        return 1;
    if (Flag((u32 *)P(p, 0x20), 0x100000))
        return 1;
    if (U16(p, 0x538) && (u16)(U16(p, 0x534) - 5) <= 1)
        return 1;
    {
        u8 mode = U8(p, 0x457);
        u8 allow = 1;
        if (mode > 1 && mode != 7)
            allow = 0;
        if (!allow)
            return 1;
    }
    return 0;
}
static inline void Facing(u8 *p, u32 value)
{
    U8(p, 0x1A) = ((value + 112) >> 5) & 7;
    U8(p, 0x19) = ((value + 96) >> 6) & 3;
}
// confidence=low: tracks a target facing and enables contact response when positions and angles agree.
#define SceneActorAnimation_UpdateTargetFacing sub_0802F298
inline static void Facing_0802F298(u8 *p, u32 value)
{
    *((u8 *)(((u8 *)p) + 0x1A)) = ((value + 112) << 24) >> 29;
    *((u8 *)(((u8 *)p) + 0x19)) = ((value + 96) >> 6) & 3;
}

void SceneActorAnimation_UpdateTargetFacing(struct Context *ctx, u8 *p)
{
    struct Vec *pos = (void *)(((u8 *)p) + 0x68);
    s32 facing;
    typeof(&gUnk_02000560) t1;
    if (ctx->target) {
        struct Vec *target = ctx->target;
        s32 angle = Math_Atan2(target->x - pos->x, target->z - pos->z);
        if (*(*((u8 **)(((u8 *)p) + 0x2B8)))) {
            u8 *current = ((u8 *)p) + 0x65;
            s32 diff = (s8)(angle - (*current));
            u32 value;
            if (diff > 8) {
                value = (*current) + 8;
                *current = value;
            } else {
                if (diff < (-8))
                    value = (*current) - 8;
                else
                    value = (*current) + diff;
                *current = value;
                value = (u16)value;
            }
            Facing_0802F298(p, value);
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
        if (((ctx->targetActor && Blocked(ctx->targetActor)) && (!(gUnk_030053F4 & 0x200))) &&
            (*((u8 *)(((u8 *)p) + 0xB)))) {
            u8 *target = ctx->targetActor;
            s32 tether = 0;
            if ((target[0x457] == 6) && TetherActor_GetPosition())
                tether = 1;
            t1 = &gUnk_02000560;
            if (tether || Flag(t1, 4))
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
        s32 near = 0;
        if (((Abs(target->x - pos->x) < radius) && (Abs(target->y - pos->y) <= 255)) &&
            (Abs(target->z - pos->z) < radius))
            near = 1;
        if (near) {
            s32 direction = (s8)Math_Atan2(pos->x - ctx->target->x, pos->z - ctx->target->z);
            s32 delta = (s8)(facing - direction);
            s32 oppositeFacing = (*((u8 *)(((u8 *)p) + 0x65))) + 128;
            u8 opposite;
            asm("" : "+r"(oppositeFacing));
            opposite = facing - oppositeFacing;
            if ((Abs(delta) <= 63) && (Abs((s8)opposite) <= 31)) {
                u32 mask = 0x800;
                *((u16 *)(((u8 *)p) + 0xE)) |= mask;
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0802F298);
#endif
