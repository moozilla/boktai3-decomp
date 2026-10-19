
#ifdef NONMATCHING
// fuzzy: 80.3%
// foundry draft (worker best 80.3%); what differs is mostly register allocation and instruction order
#define SceneActorControl_UpdateEntryTransition sub_0812F8F4
#include "global.h"
#include "scene_actor_reset.h"
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
static inline u8 SceneActorControl_TakeFlag(u8 *flag) { if (*flag) { *flag=0; return 1; } return 0; }
static inline u32 finished(struct SceneActorRegistryResetView *actor) { if (actor->field2B0) return 1; return 0; }
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
static inline void BeginControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind, u32 zero)
{
    u32 offset;
    { u8 *field=&actor->field2B2; *field=1; }
    offset=0x2B0;
    *((u8 *)actor+offset)=zero;
    actor->field2B5=kind;
    offset-=0x94;
    *(void **)((u8 *)actor+offset)=control;
    actor->field2C8=zero;
    actor->flags114 &= ~1;
    actor->word104=zero;
}
void sub_0812F8F4(struct SceneActorRegistryResetView *actor)
{
    u8 result;
    struct SceneActorRegistryControlGroup *group = &actor->group;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        BeginControl(actor,actor->entryTarget224,2,0);
    }
    if (!(u8)Callback_InvokeActor(actor, actor->update270)) {
        result = sub_0812F3CC(actor);
        if (!result) {
            if (finished(actor)) {
                SceneActorAnimation_SelectDirectionalEntry(actor,0);
                { u32 behavior = 12; u8 *field=&actor->behavior2AA; *field=behavior; }
                actor->counter2AC = result;
                actor->field2B1 = 1;
                BeginControl(actor,group->entryControl41C,26,result);
            } else {
                ++actor->counter2AC;
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0812F8F4);
#endif
