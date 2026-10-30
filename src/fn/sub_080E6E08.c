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
static inline void ClearContextFlag(u16 *flags, u32 zero)
{
    s32 mask = zero - 2;
    mask &= *flags;
    *flags = mask;
}
static inline u32 SaveStoppedControl(u8 *packet, void *control)
{
    u32 zero;
    *(u32 *)(packet + 12) = 30;
    *(void **)(packet + 16) = control;
    zero = 0;
    packet[8] = zero;
    packet[9] = zero;
    packet[10] = zero;
    return zero;
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

static inline void ClearSavedFlags(u32 *original, s32 clearMask)
{
    s32 mask = clearMask;
    register u32 *flags asm("r2");
    flags = original;
    *flags &= mask;
}
static inline void *EntryTarget(struct SceneActorRegistryResetView *actor)
{
    register u32 offset asm("r3") = 0x228;
    register void **field asm("r0");
    asm("" : "+r"(offset));
    field = (void *)((u8 *)actor + offset);
    return *field;
}
static inline void *UpdateCallback(struct SceneActorRegistryResetView *actor)
{
    u32 offset = 0x270;
    void **field;
    field = (void *)((u8 *)actor + offset);
    return *field;
}
static inline void *SavedTarget(struct SceneActorRegistryResetView *actor)
{
    register u32 offset asm("r3") = 0x22c;
    register void **field asm("r0");
    asm("" : "+r"(offset));
    field = (void *)((u8 *)actor + offset);
    return *field;
}
static inline u8 TryRouteSaved(struct SceneActorRegistryResetView *actor, void *route,
                               struct ContactPosition *position, struct ContactPosition *target,
                               u32 *before, u32 **after)
{
    s32 result = Nav_IsPositionInMesh(route, position, target);
    u32 *saved = before;
    register u16 *failedFlags asm("r0");
    *after = saved;
    if (!result) {
        register u32 offset asm("r3") = 0x2d4;
        asm("" : : "r"(offset));
        failedFlags = (void *)((u8 *)actor + offset);
        goto failed;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->body.motion, 8);
        return 1;
    }
    {
        register u32 offset asm("r1") = 0x2d4;
        asm("" : : "r"(offset));
        failedFlags = (void *)((u8 *)actor + offset);
    }
failed: {
    s32 mask = ~0x200;
    mask &= *failedFlags;
    *failedFlags = mask;
}
    return 0;
}
// confidence=low: provisional name; recovery after a hit (state 3 -> delayed restore), Mimic-owned.

void Mimic_UpdateHitRecovery(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    if (Chase_TakeArrived(actor)) {
        struct SceneActorReactionMotion *motion;
        struct SceneActorRegistryControl *control;
        u16 *flags;
        motion = (void *)&actor->tracking.target;
        motion->fieldDC = 0;
        {
            u32 o = 0x2C2;
            u16 *h;
            u32 v;
            h = (u16 *)((u8 *)actor + o);
            v = 0x40;
            *h = v;
        }
        {
            u32 offset = 0x414;
            u16 *field;
            field = (void *)((u8 *)actor + offset);
            *field = 0;
        }
        {
            u32 o = 0x418;
            u16 *h;
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
        u32 *cnt;
        asm("" : "+r"(oc));
        cnt = (u32 *)((u8 *)actor + oc);
        if ((s32)*cnt > 30) {
            u8 *group;
            u32 *cc;
            u32 one;
            register u32 z8 asm("r8");
            u32 z5;
            {
                u32 og = 0x3D4;
                asm("" : "+r"(og));
                group = (u8 *)actor + og;
            }
            {
                register u32 occ asm("r1") = 0x2CC;
                asm("" : "+r"(occ));
                cc = (u32 *)(occ + (u32)actor);
            }
            ClearSavedFlags(cc, ~0x100);
            {
                register u32 o asm("r3") = 0x3D0;
                u8 *c;
                u32 k;
                u16 *mode;
                register u32 value asm("r2");
                u32 m;
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
                m = one;
                m |= value;
                *mode = m;
            }
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
            {
                u32 three = 3;
                u32 o8 = 0x2A8;
                u32 o9;
                u32 oa;
                register u8 *a asm("r0");
                a = (u8 *)actor + o8;
                *a = three;
                o9 = 0x2A9;
                a = (u8 *)actor + o9;
                *a = one;
                oa = 0x2AA;
                a = (u8 *)actor + oa;
                *a = z8;
                o9 += 3;
                {
                    u32 *w = (u32 *)((u8 *)actor + o9);
                    *w = z5;
                }
                oa += 7;
                a = (u8 *)actor + oa;
                *a = one;
                {
                    register u32 ctl asm("r2") = (u32)SceneActorReaction_RestoreAfterDelay;
                    register u32 kind asm("r1") = 14;
                    o9 += 6;
                    a = (u8 *)actor + o9;
                    *a = one;
                    o9 -= 2;
                    a = (u8 *)actor + o9;
                    *a = z8;
                    {
                        u32 o5 = 0x2B5;
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
                    u32 m;
                    asm("" : "+r"(o));
                    fl = (u32 *)((u8 *)actor + o);
                    m = ~1;
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
                register struct ContactPosition *pos asm("r6");
                u32 *afterFlags;
                void *route;
                struct ContactPosition *tgt;
                {
                    register u32 orr asm("r2") = 0x5D0;
                    register void **rp asm("r0");
                    asm("" : "+r"(orr));
                    rp = (void **)((u8 *)actor + orr);
                    route = *rp;
                }
                pos = &actor->body.position;
                tgt = (void *)(context + 0x650);
                asm("" : "+r"(pos), "+r"(route), "+r"(tgt));
                if (TryRouteSaved(actor, route, pos, tgt, cc, &afterFlags)) {
                    u8 outside = SceneActorAttachment_TestPositionWindow(
                        (void *)actor, (struct ContactPosition *)(context + 0x650), 32, 32);
                    if (!outside) {
                        ClearSavedFlags(afterFlags, ~0xc0);
                        {
                            u8 *packet = group + 0x8c;
                            SaveControl(packet, EntryTarget(actor), 2);
                        }
                    } else {
                        ClearSavedFlags(afterFlags, ~0xc0);
                        {
                            u8 *packet = group + 0x8c;
                            u32 zero;
                            zero = SaveStoppedControl(packet, SavedTarget(actor));
                            ClearContextFlag((void *)(context + 0x65c), zero);
                        }
                    }
                    {
                        u32 offset = 0x2c2;
                        u16 *field;
                        field = (void *)((u8 *)actor + offset);
                        *field = 0x40;
                    }
                }
            }
        } else {
            Callback_InvokeActor(actor, UpdateCallback(actor));
            *cnt += 1;
        }
    }
}
