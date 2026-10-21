// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fuzzy: 92.9%
// Remaining differences: register allocation only (CpuSet argument copy, the 0x260 callback
// address temp, the speed copy slot and three offset-constant registers).
#include "scene_actor_control_helpers.h"
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Callback_InvokeActor(void *, void *);
u32 Trigger_ConsumePending(void *, u32);
void SceneActorControl_StartStoredCurve(struct SceneActorRegistryResetView *);
void CpuSet(const void *, void *, u32);
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero;
    a->field2B2 = 1;
    zero = 0;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}
static inline void Fill(void *dst, u32 *tmp, u32 value)
{
    *tmp = value;
    CpuSet(tmp, dst, 0x05000002);
}
struct SavedMotion {
    u8 pad[12];
    struct ContactPosition position;
};
static inline u8 Approach(struct SceneActorRegistryResetView *actor, struct SavedMotion *saved,
                          s32 speed)
{
    u8 *motion = (u8 *)actor + 0x50;
    s16 dx = saved->position.x - *(u16 *)(motion + 12);
    s16 dz = saved->position.z - *(u16 *)(motion + 16);
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion[9] = Math_Atan2(dx, dz);
        *(u16 *)(motion + 20) += Shift(gUnk_086149C4[(motion[9] + 64) & 255] * speed, 12);
        *(u16 *)(motion + 24) += Shift(gUnk_086149C4[motion[9]] * speed, 12);
    }
    return 0;
}
// confidence=low: homes on a stored point, then consumes a pending trigger and restarts.
void SceneActorControl_UpdateWanderTarget(struct SceneActorRegistryResetView *actor)
{
    register u8 *body asm("sb") = actor->context;
    u32 zero[1];
    if (Take(&actor->field2B1)) {
        register u8 *context asm("r5");
        register u32 zeroValue asm("r6");
        register u32 oneValue asm("r4");
        register u32 zero8 asm("r8");
        u32 base = (u32)actor;
        register u32 off asm("r3") = 0x3D0;
        u16 *field;
        u32 ctxOff;
        context = *(u8 **)(base + off);
        asm("" : "+r"(ctxOff));
        {
            register u32 ctxOffset asm("r1");
            register u16 *addr asm("r0");
            ctxOffset = 0x3E0;
            ctxOff = ctxOffset;
            addr = (void *)((u32)context + ctxOff);
            field = addr;
        }
        zero8 = 0;
        zeroValue = 0;
        *field = zeroValue;
        off += 0x13;
        {
            register u8 *flag asm("r0") = (u8 *)((u32)context + off);
            oneValue = 1;
            *flag = oneValue;
        }
        SceneActorControl_StartStoredCurve(actor);
        {
            void *cb = actor->entryTarget228;
            register u32 field asm("r2") = 0x2B2;
            register u8 *addr asm("r0");
            asm("" : "+r"(field));
            addr = (u8 *)(base + field);
            *addr = oneValue;
            off = 0x2B0;
            asm("" : "+r"(off));
            *(u8 *)(base + off) = zero8;
            off += 5;
            *(u8 *)(base + off) = oneValue;
            field = 0x21C;
            asm("" : "+r"(field));
            addr = (u8 *)(base + field);
            *(void **)addr = cb;
            off += 0x13;
            {
                register u16 *addr asm("r0") = (void *)(base + off);
                *addr = zeroValue;
            }
            {
                u32 *flags = &actor->flags114;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->word104 = zeroValue;
        }
        {
            register u8 *dst asm("r5");
            register u32 fillOff asm("r2") = 0x42C;
            asm("" : "+r"(fillOff));
            dst = context + fillOff;
            asm("" : "+r"(dst));
            Fill(dst, zero, zeroValue);
        }
        {
            register u32 bit asm("r0") = 0x800;
            register u32 fieldOffset asm("r3") = 0x2C0;
            register u16 *p asm("r2");
            u32 value;
            asm("" : "+r"(bit));
            asm("" : "+r"(fieldOffset));
            p = (void *)(base + fieldOffset);
            value = *p;
            bit |= value;
            *p = bit;
        }
    }
    SceneActorControl_TickSway(actor);
    {
        register u32 cbOff asm("r1") = 0x260;
        register s32 speed asm("r5");
        asm("" : "+r"(cbOff));
        speed = Callback_InvokeActor(actor, *(void **)((u32)actor + cbOff));
        if (Approach(actor, (void *)(body + 0x3F0), speed)) {
            if ((u8)sub_08083C38(actor)) {
                u32 base = (u32)actor;
                u32 two;
                u32 zero2;
                u32 off;
                Trigger_ConsumePending((u8 *)actor + 0xC, 1);
                two = 2;
                off = 0x2A8;
                asm("" : "+r"(off));
                zero2 = 0;
                *(u8 *)(base + off) = two;
                off = 0x2A9;
                asm("" : "+r"(off));
                *(u8 *)(base + off) = zero2;
                off += 1;
                *(u8 *)(base + off) = two;
                off = 0x2AC;
                asm("" : "+r"(off));
                *(u32 *)(base + off) = zero2;
                off = 0x2B1;
                asm("" : "+r"(off));
                *(u8 *)(base + off) = 1;
            }
        } else {
            actor->counter2AC++;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorControl_UpdateWanderTarget);
#endif
