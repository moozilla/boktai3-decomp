#include "scene_actor_reset.h"
#ifdef NONMATCHING
// fuzzy: 87.9%
// Open: register allocation (group and the 0x200 constant swap sb/r8; shape/position
// swap r5/r6) and the zero registers of the final control reset.

s32 Callback_InvokeActor(void *, void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void sub_08136F48(void *, struct SceneActorRegistryControl *);
u32 Nav_IsPositionInMesh(void *, struct ContactPosition *, void *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, void *);

static inline u8 ConsumeEntry(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

#define GROUP_FIELD(g, off, T) (*(T *)((u8 *)(g) + (off)))

static inline void ClearWordFlags(u32 *flags, u32 mask)
{
    *flags &= mask;
}

static inline void ClearHalfFlags(u16 *flags, u32 mask)
{
    *flags &= mask;
}

static inline u8 MoveTo(struct SceneActorRegistryResetView *actor, void *shape, void *destination,
                        u32 blocked)
{
    if (Nav_IsPositionInMesh(shape, &actor->position, destination)) {
        s32 result = Terrain_ApplyBlocked(shape, &actor->position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            return 1;
        }
    }
    actor->flags2D4 &= ~blocked;
    return 0;
}

// confidence=low: scene actor wait state: on entry clears mode flags; after 60 idle frames walks to the state5D4 destination.
void sub_08136F58(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group;

    if (ConsumeEntry(actor)) {
        SetControl(actor, actor->savedTarget, 0);
        ClearWordFlags(&actor->flags2CC, ~0x80);
        ClearWordFlags(&actor->flags2D0, ~0x1000000);
        ClearHalfFlags(&actor->flags2D4, ~0x200);
        actor->field2C2 = 0x40;
    }
    if ((u8)Callback_InvokeActor(actor, actor->update270) == 0) {
        u32 *counter = &actor->counter2AC;
        if (*counter == 30) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
            sub_08136F48(actor, &group->control);
        }
        if ((s32)*counter > 60) {
            u32 bit;
            SetState(actor, 0, 0, 7);
            SetControl(actor, actor->entryTarget228, 1);
            bit = 0x200;
            GROUP_FIELD(group, bit, struct ContactPosition *)->pad = 1;
            if (!MoveTo(actor, GROUP_FIELD(group, 0x1fc, void *), GROUP_FIELD(group, bit, void *),
                        bit)) {
                Callback_InvokeActor(actor, GROUP_FIELD(group, 0x224, void *));
            } else {
                struct SceneActorRegistryControl *control = &group->control;
                if (actor->flags2CC & 0x200000) {
                    ClearWordFlags(&actor->flags2D0, ~0x1000000);
                    ClearHalfFlags(&actor->flags2D4, ~0x200);
                    control->mode = 0;
                    control->counter = 0;
                    control->timer = 0;
                }
            }
        } else {
            (*counter)++;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08136F58);
#endif
