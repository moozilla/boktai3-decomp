#include "global.h"
#include "scene_actor_control_phase.h"

u8 sub_0812F3CC(u8 *);

/* Count frames while neither the actor callback at 0x270 nor
   sub_0812F3CC takes over (the taken flag's empty test as in Ivy). */
void sub_08131AF8(u8 *p)
{
    if (SceneActorControl_TakeFlag(&p[0x2B1])) {
    }
    if (!(u8)Callback_InvokeActor(p, *(void **)(p + 0x270)) && !sub_0812F3CC(p))
        (*(s32 *)(p + 0x2AC))++;
}
