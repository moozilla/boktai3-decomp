#include "engine/shared/global.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
#include "actors/scene_combat_actor/scene_combat_actor.h"
#include "engine/shared/scene_input_widget.h"

#define gActorStateCallbacks gUnk_0824EF10

extern u32 gUnk_02000560;
extern void *gUnk_0824EF10[];

void ActorController_UpdateAnimation(struct SceneCombatActor *p)
{
    u8 *finished;
    struct SceneInputWidget *anim;
    u32 mask = 0x400;
    if (!((gEngineFlags | gScenePauseFlags) & mask)) {
        u32 four = 4;
        if (!(gUnk_02000560 & four))
            Callback_InvokeActor(p, gUnk_0824EF10[p->action]);
    }
    finished = &p->animationComplete;
    *finished = 0;
    anim = (void *)((u8 *)p + 0xB4);
    if (Anim_Step(anim, (u8 *)p + 0x94))
        *finished = 1;
    {
        if (p->animationStatus & 1)
            *finished = 1;
        if (p->flags & 0x800)
            anim->flags |= 1;
        else
            anim->flags &= ~1;
    }
    if (p->flags & 0x400)
        anim->flags |= 0x200;
    else
        anim->flags &= ~0x200;
    p->actionCounter++;
}
