#include "scene_actor_control.h"
#include "ram.h"
#include "scene_actor_control_step.h"
struct SceneActorControlManager *SceneActorControl_EnsureSingleton(void);
struct SceneActorControlSlot *SceneActorControl_AllocateSlot(struct SceneActorControlManager *);
void SceneActorControl_DecodeDirection(const void *, u8 *, u8 *);
s32 Region_FindContaining(const struct ContactPosition *);
void Sprite_InitUnlinked(void *, void *, u32);
u32 AreaEffect_HasEnabledTerrainValue(s32);
void SceneActorControl_InitCollider(struct SceneActorControlSlot *, s32, s32);
s32 SceneActorControl_CreateFromPosition(const struct SceneActorControlInput *input)
{
    struct SceneActorControlManager *found = gSceneActorControlManager;
    struct SceneActorControlManager *p;
    struct SceneActorControlSlot *slot, *sprite;
    u8 unused, sequence;
    if (!found) {
        p = SceneActorControl_EnsureSingleton();
        if (!p)
            return -1;
    } else
        p = found;
    slot = SceneActorControl_AllocateSlot(p);
    if (!slot)
        return -1;
    slot->kind = 1;
    SceneActorControl_DecodeDirection(input, &unused, &sequence);
    slot->savedPosition = input->position;
    slot->scene = Region_FindContaining(&slot->savedPosition);
    slot->flags = 0;
    slot->parameter = 0;
    sprite = slot;
    slot->position = slot->savedPosition;
    Sprite_InitUnlinked(slot, p->graphics, 0);
    Anim_SetSequence(&slot->sequence, p->sequenceResource, 1, 0, sequence);
    SceneActorControl_StepSprite(slot, sprite, &slot->sequence);
    sprite->spriteFlags &= ~1;
    sprite->priority = 1;
    if (AreaEffect_HasEnabledTerrainValue(slot->scene))
        slot->flags |= 0x20;
    SceneActorControl_InitCollider(slot, 0, 0);
    return slot->index;
}
