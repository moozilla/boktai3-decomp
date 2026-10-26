#define ScriptedColliderActors_EnsureManager FragmentSpritePool_GetOrCreate
#define ScriptedColliderActor_InitCollider   LargeFragmentEffect_InitCollider
#include "global.h"
#include "engine.h"
#include "scene_vec.h"
#include "ram.h"
struct ScriptedColliderActor {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *graphics;
    u8 unknown10[0xc];
    struct SceneVec position;
    u8 unknown24[8];
    u8 animation[0x10];
    u8 collider[0x54];
    void *script;
    u16 timer;
    u8 state, unknown97;
    s16 variant;
    u8 unknown9a[5], entered;
};
struct ScriptedColliderManager {
    u8 task[0x18];
    u8 resource[0x1c];
};
struct ScriptedColliderManager *FragmentSpritePool_GetOrCreate(void);
struct ScriptedColliderActor *SceneSlot_Allocate(struct ScriptedColliderManager *);
void LargeFragmentEffect_InitCollider(void *, u32, u32, u32, u32, u32, u32, u32);
void ScriptedColliderActor_Create(struct SceneVec *position, u32 variant, u32 a, u32 b, u32 c,
                                  u32 d)
{
    struct ScriptedColliderManager *manager = gScriptedColliderManager;
    struct ScriptedColliderActor *actor;
    if (!manager) {
        manager = FragmentSpritePool_GetOrCreate();
        if (!manager)
            return;
    }
    actor = SceneSlot_Allocate(manager);
    if (!actor)
        return;
    actor->variant = variant;
    if (actor->variant == 0)
        actor->state = 2;
    else
        actor->state = 1;
    actor->timer = 0;
    actor->entered = 1;
    actor->position = *position;
    actor->script = ScriptProc_Start(0x922e, 0x121b);
    Sprite_Init(actor, manager->resource, 0);
    Anim_SetSequence(actor->animation, actor->script, 6, 0, 0);
    AnimData_SelectPaletteBank(actor->graphics, 0x127);
    actor->priority = 2;
    actor->flags |= 1;
    LargeFragmentEffect_InitCollider(actor, a, b, c, d, 0, 0, 0);
}
