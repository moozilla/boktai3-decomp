#include "scene_actor_control_motion.h"
#include "ram.h"
#include "scene_actor_placement.h"

extern u8 *gUnknown_02000580[];

static inline s32 TruncateShift(s32 value, u32 shift)
{
    return value >= 0 ? value >> shift : -((-value) >> shift);
}

static inline void SetHeightFromGround(struct SceneActorRegistryResetView *actor)
{
    u32 ground = ((struct SceneActorControlGroundView *)actor)->ground;
    actor->body.position.y = actor->field2EA + ground;
}

// confidence=low: adds the hover offset to the actor and bobs its drawable.
void Lun_UpdateHoverHeight(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    s32 delta, magnitude;
    s32 cap;

    if (SceneActorPlacement_IsUnavailable((void *)actor))
        return;
    {
        u32 mask = 1;
        if (SceneActorPlacement_TestValue(state->flags3D4, mask))
            return;
    }
    if ((u8)(actor->field2B5 - 0x23) <= 1) {
        SetHeightFromGround(actor);
        return;
    }
    if (!SceneActorPlacement_ReadFlags32((void *)actor, 0x100000)) {
        SetHeightFromGround(actor);
        return;
    }
    {
        u32 passageActive;
        u32 mode = gPlayerPassageState;
        passageActive = 0;
        if (mode != 0 && gPlayerInteractionMode == 5)
            passageActive = 1;
        if (passageActive || gUnknown_02000580[0][0x418] == 4)
            cap = 0x3c;
        else
            cap = 0x104;
    }
    if ((s16)actor->field2EA < cap) {
        actor->field2EA += TruncateShift(gUnk_086149C4[state->phase], 9);
        if (state->phase <= 0x3f)
            state->phase++;
        actor->body.position.y =
            ((struct SceneActorControlGroundView *)actor)->ground + actor->field2EA;
    }
    delta = ((struct SceneActorControlGroundView *)actor)->ground + cap - actor->body.position.y;
    magnitude = delta >= 0 ? delta : -delta;
    if (magnitude > 0x10) {
        if (delta < 0)
            actor->field2EA -= 2;
        else
            actor->field2EA += 2;
    }
    SetHeightFromGround(actor);
    {
        struct SceneActorRegistrySequenceDrawable *drawable = actor->drawable;
        drawable->position.y += TruncateShift(gUnk_086149C4[state->phase] * 32, 12);
    }
    state->phase += 2;
}
