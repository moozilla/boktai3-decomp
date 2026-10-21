// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fuzzy: 98.7%
// Remaining differences: CpuSet size constant load order and the zero offset register
// in the second sine-table read (r3 vs r1).
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
    register s32 spd asm("r5") = speed;
    s16 dx = saved->position.x - *(u16 *)(motion + 12);
    s16 dz = saved->position.z - *(u16 *)(motion + 16);
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion[9] = Math_Atan2(dx, dz);
        *(u16 *)(motion + 20) += Shift(gUnk_086149C4[(motion[9] + 64) & 255] * spd, 12);
        *(u16 *)(motion + 24) += Shift(gUnk_086149C4[motion[9]] * spd, 12);
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
            register u32 fillOff asm("r2") = 0x42C;
            register u8 *dst asm("r5");
            asm("" : "+r"(fillOff));
            dst = context + fillOff;
            asm("" : "+r"(dst));
            zero[0] = zeroValue;
            {
                CpuSet(zero, dst, 0x05000002);
            }
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
        s32 speed;
        void *cb;
        asm("" : "+r"(cbOff));
        {
            register void **cbp asm("r0");
            cbp = (void **)((u32)actor + cbOff);
            cb = *cbp;
        }
        speed = Callback_InvokeActor(actor, cb);
        if (Approach(actor, (void *)(body + 0x3F0), speed)) {
            if ((u8)sub_08083C38(actor)) {
                u32 base = (u32)actor;
                register u32 two asm("r1");
                register u32 zero2 asm("r2");
                register u32 off asm("r3");
                Trigger_ConsumePending((u8 *)actor + 0xC, 1);
                two = 2;
                {
                    register u32 o asm("r2") = 0x2A8;
                    register u8 *a asm("r0");
                    asm("" : "+r"(o));
                    a = (u8 *)(base + o);
                    zero2 = 0;
                    *a = two;
                }
                off = 0x2A9;
                asm("" : "+r"(off));
                {
                    register u8 *a3 asm("r0") = (u8 *)(base + off);
                    *a3 = zero2;
                }
                off += 1;
                {
                    register u8 *a4 asm("r0") = (u8 *)(base + off);
                    *a4 = two;
                }
                {
                    register u32 o asm("r1") = 0x2AC;
                    register u32 *a asm("r0");
                    asm("" : "+r"(o));
                    a = (u32 *)(base + o);
                    *a = zero2;
                }
                {
                    register u32 o2 asm("r2") = 0x2B1;
                    register u8 *a2 asm("r1");
                    asm("" : "+r"(o2));
                    a2 = (u8 *)(base + o2);
                    *a2 = 1;
                }
            }
        } else {
            register u32 o asm("r3") = 0x2AC;
            register u32 *a asm("r1");
            u32 v;
            asm("" : "+r"(o));
            a = (u32 *)((u32)actor + o);
            v = *a;
            v++;
            *a = v;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorControl_UpdateWanderTarget);
#endif
