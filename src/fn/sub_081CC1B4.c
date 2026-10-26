#include "global.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
struct S {
    u8 f0;
    u8 b1;
    u8 f2[5];
    u8 b7;
    u8 f8;
    u8 b9;
    u8 fa[4];
    u16 flags;
    u8 g[0x2e4 - 0x10];
    void *cb;
};
s32 Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
s32 Callback_InvokeActor(void *, void *);
s32 sub_081C8FB0(void *, void *, s32);
#define B(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(u32 *)((u8 *)(p) + (o)))
struct Position {
    s16 x, y, z, pad;
};
s32 Math_Atan2(s32, s32);
extern u32 gUnk_02000560;
void *TetherActor_GetPosition(void);
void Actor_SetAttachmentEffect(void *, u32, u32, u32);
static inline u32 MaskWord(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u32 Event(u32 mask)
{
    return gUnk_02000560 & mask;
}
static inline u32 Tethered(void *a)
{
    register u32 pointerOffset asm("r1") = 0x438;
    u8 *target;
    u32 kindOffset;
    u32 result;
    target = *(u8 **)((u8 *)a + pointerOffset);
    asm("" ::"r"(pointerOffset));
    kindOffset = 0x457;
    if (target[kindOffset] == 6) {
        void *position = TetherActor_GetPosition();
        result = 1;
        if (position)
            goto done;
    }
    result = 0;
done:
    return result;
}
static inline u16 MenuState(u8 *target)
{
    asm("" ::: "r3");
    {
        register u32 offset asm("r3") = 0x534;
        u16 value;
        value = H(target, offset);
        asm("" ::"r"(offset));
        return value;
    }
}
static inline u8 MenuKind(u8 *target)
{
    register u32 offset asm("r5") = 0x457;
    {
        u8 value = target[offset];
        asm("" ::"r"(offset));
        return value;
    }
}
static inline void *CurrentCallback(struct S *p)
{
    u32 offset = 0x2E4;
    {
        void *callback = *(void **)((u8 *)p + offset);
        return callback;
    }
}
// confidence=low: turns toward a scene target and refreshes its attachment animation.
#define StateActor_UpdateTargetTurn sub_081CC1B4
void StateActor_UpdateTargetTurn(void *a, struct S *p)
{
    u32 r;
    u8 hit;
    {
        struct Position *target = *(struct Position **)((u8 *)a + 0x434);
        if (target) {
            struct Position *position = (struct Position *)((u8 *)p + 0x68);
            s32 desired = Math_Atan2(target->x - position->x, target->z - position->z);
            if (**(u8 **)((u8 *)p + 0x2B8)) {
                register u8 *angle asm("r2") = (u8 *)p + 0x65;
                s32 difference = (s8)(desired - *angle);
                register s32 next asm("r1");
                if (difference > 8) {
                    next = *angle + 8;
                    *angle = next;
                } else {
                    if (difference < -8)
                        next = *angle - 8;
                    else
                        next = *angle + difference;
                    *angle = next;
                    next = (u16)next;
                }
                {
                    s32 t = (next + 0x70) >> 5;
                    u32 mask = 7;
                    mask &= t;
                    B(p, 0x1A) = mask;
                }
                {
                    s32 t = (next + 0x60) >> 6;
                    u32 mask = 3;
                    mask &= t;
                    B(p, 0x19) = mask;
                }
            }
        }
    }
    if (p->b9 != 0) {
        p->b9 = 0;
        p->b7 = 0;
        switch (p->b1) {
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
                r = 0;
                break;
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
            case 27:
            case 28:
                r = 0;
                break;
            case 29:
                r = 0xb;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
        B(p, 0x1F0) = 1;
        {
            u8 angle = B(p, 0x65);
            B(p, 0x225) = angle;
        }
        {
            register u8 *rawTarget asm("r0") = *(u8 **)((u8 *)a + 0x438);
            u8 *target;
            if (!rawTarget)
                goto attachmentDone;
            target = rawTarget;
            if ((u8)(target[0x418] - 3) > 2 && !MaskWord((u32 *)(target + 0x20), 0x100000) &&
                (!H(target, 0x538) || (u16)(MenuState(target) - 5) > 1)) {
                u8 kind = MenuKind(target);
                u8 allow = 1;
                if (kind > 1 && kind != 7)
                    allow = 0;
                if (!allow)
                    goto restricted;
                goto attachmentDone;
            }
        restricted:
            if (!(gEngineFlags & 0x200) && B(p, 0xB)) {
                if (Tethered(a) || Event(4))
                    Actor_SetAttachmentEffect(p, 0, 0, 0);
                else
                    Actor_SetAttachmentEffect(p, 3, 0, 0);
            }
        }
    attachmentDone:;
    } else {
        switch (p->b1) {
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
                r = 0;
                break;
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
            case 27:
            case 28:
                r = 0;
                break;
            case 29:
                r = 0xb;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 1);
    }
    {
        u32 m = 0x1000;
        p->flags = m | p->flags;
    }
    hit = 0;
    if ((u8)Callback_InvokeActor(p, CurrentCallback(p)) != 1 && sub_081C8FB0(a, p, 0)) {
        hit = 1;
        {
            u32 m = 0x800;
            p->flags = m | p->flags;
        }
    }
}
