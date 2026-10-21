// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_attachment.h"

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
static inline void SaveControl(u8 *packet, void *control, u8 behavior)
{
    *(u32 *)(packet + 12) = 30;
    *(void **)(packet + 16) = control;
    packet[8] = 0;
    packet[9] = 0;
    packet[10] = behavior;
}
static inline void clearFlag(u32 *flags)
{
    u32 mask = ~1;
    *flags &= mask;
}
static inline void clearRouteFlag(u32 *flags)
{
    u32 mask = ~0x1000000;
    *flags &= mask;
}
static inline u32 testFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}

#ifdef NONMATCHING
// fuzzy: 88.4%
// Remaining differences: pinned-offset load order around Chase_TryRoute (route object load, cc copy in r8),
// the flags2D4 clear offsets and the zero/mask registers in the SaveControl tails.
// confidence=low: provisional name; recovery after a hit (state 3 -> delayed restore), Mimic-owned.
#define Mimic_UpdateHitRecovery sub_080E6E08

void Mimic_UpdateHitRecovery(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    if (Chase_TakeArrived(actor)) {
        struct SceneActorReactionMotion *motion;
        struct SceneActorRegistryControl *control;
        u16 *flags;
        motion = (void *)&actor->fieldDC;
        motion->fieldDC = 0;
        {
            register u32 o asm("r3") = 0x2C2;
            register u16 *h asm("r1");
            register u32 v asm("r0");
            asm("" : "+r"(o));
            h = (u16 *)((u8 *)actor + o);
            v = 0x40;
            asm("" : "+r"(v));
            *h = v;
        }
        actor->group.flags414 = 0;
        {
            register u32 o asm("r3") = 0x418;
            register u16 *h asm("r0");
            asm("" : "+r"(o));
            h = (u16 *)((u8 *)actor + o);
            *h = 0;
        }
        flags = &actor->flags2D4;
        {
            u32 mask = ~0x380;
            mask &= *flags;
            *flags = mask;
        }
        {
            s32 mask = ~0x18;
            mask &= motion->flags;
            motion->flags = mask;
        }
        control = &actor->group.control;
        if (testFlags(actor, 0x200000)) {
            clearRouteFlag(&actor->flags2D0);
            {
                u32 mask = ~0x200;
                mask &= *flags;
                *flags = mask;
            }
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        {
            u32 mask = ~0x400;
            mask &= *flags;
            *flags = mask;
        }
        actor->field5DA = 0xFF;
    }
    {
        register u32 oc asm("r3") = 0x2AC;
        register u32 *cnt asm("r4");
        asm("" : "+r"(oc));
        cnt = (u32 *)((u8 *)actor + oc);
        if ((s32)*cnt > 30) {
            u8 *group;
            u32 *cc;
            register u32 one asm("r4");
            register u32 z8 asm("r8");
            register u32 z5 asm("r5");
            {
                register u32 og asm("r0") = 0x3D4;
                asm("" : "+r"(og));
                group = (u8 *)actor + og;
            }
            {
                register u32 occ asm("r1") = 0x2CC;
                asm("" : "+r"(occ));
                cc = (u32 *)(occ + (u32)actor);
            }
            {
                register u32 *p asm("r2") = cc;
                u32 mask = ~0x100;
                *p &= mask;
            }
            {
                register u32 o asm("r3") = 0x3D0;
                register u8 *c asm("r1");
                register u32 k asm("r0");
                register u16 *mode asm("r1");
                register u32 value asm("r2");
                register u32 m asm("r0");
                asm("" : "+r"(o));
                {
                    register u8 **cp asm("r0") = (u8 **)((u8 *)actor + o);
                    c = *cp;
                }
                one = 1;
                k = 0x65C;
                asm("" : "+r"(k));
                mode = (u16 *)(c + k);
                value = *mode;
                z8 = 0;
                z5 = 0;
                asm("" : "+r"(z8), "+r"(z5));
                m = one;
                m |= value;
                *mode = m;
            }
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
            {
                register u32 three asm("r1") = 3;
                register u32 o8 asm("r2") = 0x2A8;
                register u32 o9 asm("r3");
                register u32 oa asm("r1");
                register u8 *a asm("r0");
                asm("" : "+r"(three), "+r"(o8));
                a = (u8 *)actor + o8;
                *a = three;
                o9 = 0x2A9;
                asm("" : "+r"(o9));
                a = (u8 *)actor + o9;
                *a = one;
                oa = 0x2AA;
                asm("" : "+r"(oa));
                a = (u8 *)actor + oa;
                *a = z8;
                o9 += 3;
                asm("" : "+r"(o9));
                {
                    register u32 *w asm("r0") = (u32 *)((u8 *)actor + o9);
                    *w = z5;
                }
                oa += 7;
                asm("" : "+r"(oa));
                a = (u8 *)actor + oa;
                *a = one;
                {
                    register u32 ctl asm("r2") = (u32)SceneActorReaction_RestoreAfterDelay;
                    register u32 kind asm("r1") = 14;
                    asm("" : "+r"(ctl), "+r"(kind));
                    o9 += 6;
                    asm("" : "+r"(o9));
                    a = (u8 *)actor + o9;
                    *a = one;
                    o9 -= 2;
                    asm("" : "+r"(o9));
                    a = (u8 *)actor + o9;
                    *a = z8;
                    {
                        register u32 o5 asm("r3") = 0x2B5;
                        asm("" : "+r"(o5));
                        a = (u8 *)actor + o5;
                        *a = kind;
                    }
                    {
                        register u32 ot asm("r1") = 0x21C;
                        register u32 *w asm("r0");
                        asm("" : "+r"(ot));
                        w = (u32 *)((u8 *)actor + ot);
                        *w = ctl;
                    }
                }
                {
                    register u32 o asm("r2") = 0x2C8;
                    register u16 *h asm("r0");
                    asm("" : "+r"(o));
                    h = (u16 *)((u8 *)actor + o);
                    *h = z5;
                }
                {
                    register u32 o asm("r3") = 0x114;
                    register u32 *fl asm("r2");
                    register u32 m asm("r1");
                    asm("" : "+r"(o));
                    fl = (u32 *)((u8 *)actor + o);
                    m = ~1;
                    asm("" : "+r"(m));
                    *fl = m & *fl;
                }
                {
                    register u32 o asm("r1") = 0x104;
                    register u32 *w asm("r0");
                    asm("" : "+r"(o));
                    w = (u32 *)((u8 *)actor + o);
                    *w = z5;
                }
            }
            {
                register struct ContactPosition *pos asm("r6") = &actor->position;
                register void *route asm("r5");
                register struct ContactPosition *tgt asm("r4") =
                    (struct ContactPosition *)(context + 0x650);
                {
                    register u32 orr asm("r2") = 0x5D0;
                    register void **rp asm("r0");
                    asm("" : "+r"(orr));
                    rp = (void **)((u8 *)actor + orr);
                    asm("" : "+r"(rp));
                    route = *rp;
                }
                asm("" : "+r"(pos), "+r"(route), "+r"(tgt));
                if (Chase_TryRoute(actor, route, pos, tgt)) {
                    u8 outside = SceneActorAttachment_TestPositionWindow(
                        (void *)actor, (struct ContactPosition *)(context + 0x650), 32, 32);
                    if (!outside) {
                        register u32 *flags asm("r2") = cc;
                        u32 mask = ~0xC0;
                        *flags &= mask;
                        SaveControl(group + 0x8c, actor->entryTarget228, 2);
                    } else {
                        register u32 *flags asm("r2") = cc;
                        u32 mask = ~0xC0;
                        *flags &= mask;
                        SaveControl(group + 0x8c, actor->savedTarget, 0);
                        *(u16 *)(context + 0x65C) &= ~1;
                    }
                    actor->field2C2 = 0x40;
                }
            }
        } else {
            Callback_InvokeActor(actor, actor->update270);
            *cnt += 1;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080E6E08);
#endif
