#include "scene_actor_reset.h"
// MATCHING_CONCESSION: register-pinning

// foundry draft (worker best 85.4%); what differs is mostly register allocation and instruction order
void *SceneActorContacts_FindObjectById(u16);
void SceneActorMovement_ResolveRoutePosition(struct SceneActorRegistryActor *);
void sub_080A4798(struct SceneActorRegistryResetView *);
s32 SceneActorPlacement_BeginContactRoute(struct SceneActorRegistryResetView *p)
{
    struct SceneActorRegistryControlGroup *group;
    void *control;
    s32 result;
    s32 offset;
    if ((s32)p->field304 > 0)
        goto begin;
    result = 0;
    goto done;
begin: {
    register u32 goff asm("r2") = 0x3D4;
    asm("" : "+r"(goff));
    group = (struct SceneActorRegistryControlGroup *)((u8 *)p + goff);
}
    p->tracking.target = (u32)SceneActorContacts_FindObjectById(p->field304);
    {
        u16 behavior = 12;
        u8 *field;
        u32 zero;
        u32 phaseOffset;
        u32 counterOffset;
        u32 one;
        offset = 0x2A8;
        field = (u8 *)p + offset;
        zero = 0;
        *field = zero;
        phaseOffset = 0x2A9;
        *((u8 *)p + phaseOffset) = zero;
        offset += 2;
        *((u8 *)p + offset) = behavior;
        counterOffset = 0x2AC;
        *(u32 *)((u8 *)p + counterOffset) = zero;
        phaseOffset += 8;
        field = (u8 *)p + phaseOffset;
        one = 1;
        *field = one;
        control = group->gate.control;
        {
            u32 kind = 26;
            offset += 8;
            *((u8 *)p + offset) = one;
            counterOffset = 0x2B0;
            *((u8 *)p + counterOffset) = zero;
            offset += 3;
            field = (u8 *)p + offset;
            *field = kind;
            counterOffset -= 0x94;
            field = (u8 *)p + counterOffset;
            *(void **)field = control;
            phaseOffset = 0x2C8;
            asm("" : "+r"(phaseOffset));
            field = (u8 *)p + phaseOffset;
            *(u16 *)field = zero;
            {
                u32 *flags;
                u32 mask;
                offset = 0x114;
                asm("" : "+r"(offset));
                flags = (u32 *)((u8 *)p + offset);
                mask = ~1;
                *flags &= mask;
            }
            p->word104 = zero;
        }
    }
    sub_080A4798(p);
    SceneActorMovement_ResolveRoutePosition((void *)p);
    {
        u16 *flags;
        u32 mask;
        u32 value;
        offset = 0x414;
        flags = (u16 *)((u8 *)p + offset);
        mask = 4;
        value = *flags;
        mask |= value;
        *flags = mask;
        {
            u32 mask = 128;
            u16 *flags = &p->flags2D4;
            u32 value = *flags;
            mask |= value;
            *flags = mask;
        }
        {
            register u32 voff asm("r2") = 0x416;
            register u16 *vp asm("r0");
            u32 value;
            u16 *out;
            asm("" : "+r"(voff));
            vp = (u16 *)((u8 *)p + voff);
            value = *vp;
            offset += 4;
            out = (u16 *)((u8 *)p + offset);
            *out = value;
        }
    }
    result = 1;
done:
    return result;
}
