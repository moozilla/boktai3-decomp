#include "global.h"
#include "scene_actor_reset.h"

s32 Callback_InvokeActor(void *, void *);

s32 SceneStimulus_FindOverlap(s32, s32, u32, u32, u32);
u32 SceneActorContacts_ReleaseNode(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorPosition_ResolveTaggedPosition(void *, void *, void *, s32);
s32 Terrain_ApplyBlocked(void *, void *, void *);
s32 Nav_IsPositionInMesh(void *, void *, void *);
void SceneActorAttachment_RetargetNearestPlayer(void *, void *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);

#ifdef NONMATCHING
// fuzzy: 40.2%
// scope shape 289 (differences once low registers are set aside)
// confidence=low: per-frame contact probe that clears or re-arms the actor's terrain/route state; returns 1 while the probe is active.
s32 sub_0807F130(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryControlGroup *group;
    struct SceneActorRegistryControl *control;
    struct ContactPosition *position;
    struct ContactPosition *state;
    void *route;
    u32 hit;
    s32 overlap;
    s32 cell;
    u8 kind;
    u8 *p;
    s32 cx, cy, val;
    s32 target;
    s32 r;
    s32 stop;

    group = &actor->group;
    control = &group->control;
    actor->flags2D0 &= 0xFEFFFFFF;
    stop = (!(actor->flags2CC & 0x200000) || (actor->flags2D0 & 4) || (actor->flags2CC & 0x100) ||
            (actor->flags2D4 & 0xE13F) || (actor->flagsEB & 0x10) || actor->field2B8 ||
            *(u16 *)((u8 *)actor + 0x178))
               ? 1
               : 0;
    if (stop)
        return 0;
    control->mode = stop;
    position = (struct ContactPosition *)&actor->position;
    hit = 0;
    overlap = (s8)SceneStimulus_FindOverlap(position->x, position->z, *(u16 *)((u8 *)control + 2),
                                            *((u8 *)control + 0xB), actor->id & 0xFFFF);
    if (overlap < 0)
        return 0;
    kind = (u8)overlap;
    if (control->timer == 0) {
        actor->flags2CC &= 0xFFFEBE3F;
        *(u32 *)((u8 *)control + 0x18) = hit;
        *(u16 *)((u8 *)control + 0x14) = 0;
        control->timer = 180;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->category);
        target = *(u32 *)((u8 *)group + 0x68);
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = 9;
        actor->target = (void *)target;
        actor->field2C8 = hit;
        actor->flags114 &= -2;
        actor->word104 = hit;
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 11;
        actor->counter2AC = hit;
        actor->field2B1 = 1;
        hit = 1;
    }
    if (actor->flags2D4 & 0x1000) {
        SceneActorContacts_ReleaseNode(*(void **)((u8 *)actor + 0x45C));
        actor->flags2D4 &= ~0x1000;
        *(u32 *)((u8 *)actor + 0x45C) = stop;
    }
    SceneActorPosition_ResolveTaggedPosition(control, &actor->id, actor->routeObject, (s8)kind);
    if (actor->flags2D4 & 0x80) {
        group->field418 += 0x12C;
        if (group->field418 > group->field416)
            group->field418 = group->field416;
    }
    state = (struct ContactPosition *)actor->state5D4;
    route = actor->routeObject;
    {
        u16 *src = *(u16 **)((u8 *)group + 0x74 + 8);
        ((u16 *)state)[0] = src[0];
        ((u16 *)state)[2] = src[1];
    }
    cx = (s8)(position->x >> 8);
    cy = (s8)(position->y >> 8);
    if (cx < 0 || cy < 0 || (u32)cx >= gUnk_030052F8 || (u32)cy >= gUnk_030052FC)
        cell = 0;
    else
        cell = *(u16 *)((u8 *)gUnk_030052F4 + 0x24 + cy * 2) + cx;
    p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        p += 4;
    else
        p = *(u8 **)((u8 *)gUnk_030052F4 + 4) + cell * 4 + 12;
    val = (*p & 0xF) << 8;
    switch (*p >> 4) {
        case 1:
            val -= *((u8 *)state + 4);
            break;
        case 2:
            val -= *((u8 *)state + 0);
            break;
    }
    ((u16 *)state)[1] = val;
    ((u16 *)state)[1] = position->y;
    ((u16 *)state)[3] = 0;
    route = actor->routeObject;
    target = (s32)((u8 *)group + 0x58);
    r = 0;
    if (!Nav_IsPositionInMesh(route, position, (void *)target)) {
        actor->flags2D4 &= ~0x200;
    } else {
        s32 res = Terrain_ApplyBlocked(route, position, (void *)target);
        if (res == 0) {
            actor->flags2D4 &= ~0x200;
        } else {
            if (res != 2)
                Memory_Clear(&actor->motion, 8);
            r = 1;
        }
    }
    if (r == 0) {
        Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x5F8));
        return 1;
    }
    actor->flags2CC |= 0x10;
    actor->flags2D4 |= 0x200;
    actor->field2C2 = 0x20;
    actor->flags2D0 |= 0x01000000;
    SceneActorAttachment_RetargetNearestPlayer(actor, (void *)target);
    if (hit)
        return 1;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807F130);
#endif
