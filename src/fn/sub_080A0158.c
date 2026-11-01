// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "display_sequence_inline.h"

struct CrowResetContext {
    u8 unknown0[0x3D4];
    u32 flags3D4;
    u8 field3D8, unknown3D9[4], field3DD;
    u8 unknown3DE[6];
    u8 field3E4;
    u8 unknown3E5[3];
    u16 cell3E8;
    u8 unknown3EA[4];
    u8 field3EE, field3EF;
    u8 unknown3F0[0x2C];
    struct ContactPosition vectors[3];
    struct ContactPosition storedPosition;
};

struct CrowPlacement {
    u32 xy, z;
};

/* Crow ground-state reset (tentative): clears the context's state, restores the stored position,
 * resnaps the actor to the terrain height and steps the display sequence. */
void Crow_ResetGroundState(struct SceneActorRegistryResetView *actor)
{
    struct CrowResetContext *context = actor->context;
    u32 zero;
    context->field3D8 = 0;
    context->field3EE = 0;
    context->field3EF = 0;
    context->field3DD = 0;
    context->field3E4 = 0;
    context->flags3D4 = 0;
    *(struct CrowPlacement *)&actor->body.position =
        *(struct CrowPlacement *)&context->storedPosition;
    actor->terrainCell = context->cell3E8;
    actor->groundHeight2BE =
        SceneActorTerrain_GetHeight(actor->terrainCell, &context->storedPosition);
    actor->field2EA = actor->body.position.y;
    actor->body.position.y = actor->groundHeight2BE;
    if (!actor->drawableKind)
        *(struct CrowPlacement *)((u8 *)actor->drawable + 0x1C) =
            *(struct CrowPlacement *)&actor->body.position;
    else
        *(struct CrowPlacement *)((u8 *)actor->drawable + 0x40) =
            *(struct CrowPlacement *)&actor->body.position;
    {
        void *dst = &context->vectors[0];
        void *src = &zero;
        u32 control = 0x05000002;
        zero = 0;
        asm("" : "+r"(src));
        asm("" : "+r"(control));
        CpuSet(src, dst, control);
    }
    {
        void *dst = &context->vectors[1];
        void *src = &zero;
        u32 control = 0x05000002;
        zero = 0;
        asm("" : "+r"(src));
        asm("" : "+r"(control));
        CpuSet(src, dst, control);
    }
    {
        void *dst = &context->vectors[2];
        void *src = &zero;
        u32 control = 0x05000002;
        zero = 0;
        asm("" : "+r"(src));
        asm("" : "+r"(control));
        CpuSet(src, dst, control);
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    DisplaySequence_Step(actor->drawable);
}
