#include "scene_actor_control.h"
#include "ram.h"
#include "scene_actor_control_step.h"
/* SceneActorControl_CreateFromScript: Creates an actor slot using script position and flip parameters and initializes its sprite and collider */
#define SceneActorControl_CreateFromScript sub_08067990
#define SceneActorControl_CreateFromScript sub_08067990
struct SceneActorControlManager *SceneActorControl_EnsureSingleton(void);
struct SceneActorControlSlot *SceneActorControl_AllocateSlot(struct SceneActorControlManager *);
s32 Region_FindContaining(const struct ContactPosition *);
void Sprite_InitUnlinked(void *, void *, u32);
u32 AreaEffect_HasEnabledTerrainValue(s32);
void SceneActorControl_InitCollider(struct SceneActorControlSlot *, s32, s32);
void SceneActorControl_CreateFromScript(void)
{
    struct SceneActorControlManager *p = gSceneActorControlManager;
    struct SceneActorControlSlot *slot, *sprite;
    u8 flip;
    if (!p) {
        p = SceneActorControl_EnsureSingleton();
        if (!p)
            return;
    }
    slot = SceneActorControl_AllocateSlot(p);
    if (!slot)
        return;
    slot->kind = 1;
    if (Script_SeekToKeyword(0x70)) {
        slot->savedPosition.x = Script_GetPc() ? Script_GetValue() : 0;
        slot->savedPosition.y = Script_GetPc() ? Script_GetValue() : 0;
        slot->savedPosition.z = Script_GetPc() ? Script_GetValue() : 0;
    } else {
        slot->savedPosition.x = 0;
        slot->savedPosition.y = 0;
        slot->savedPosition.z = 0;
    }
    flip = Script_GetKeywordValue(0x72, 0);
    slot->scene = Region_FindContaining(&slot->savedPosition);
    slot->flags = 0;
    slot->parameter = 0;
    sprite = slot;
    slot->position = slot->savedPosition;
    Sprite_InitUnlinked(slot, p->graphics, 0);
    Anim_SetSequence(&slot->sequence, (void *)p->sequenceResource, 1, 0, flip);
    SceneActorControl_StepSprite(slot, sprite, &slot->sequence);
    sprite->spriteFlags &= ~1;
    sprite->priority = 1;
    if (AreaEffect_HasEnabledTerrainValue(slot->scene))
        slot->flags |= 0x20;
    SceneActorControl_InitCollider(slot, 0, 0);
}
