#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"
s32 Callback_InvokeActor(void *, void *);
s32 sub_081C8FB0(void *, void *, s32);
void StoryCharacter_StartAnimStep(void *context, struct SceneActor *p)
{
    u32 animation;
    if (p->animationEntered != 0) {
        p->animationEntered = 0;
        p->finished = 0;
        switch (p->variant) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
                animation = 0;
                break;
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
                animation = 0;
                break;
            case 29:
                animation = 0xb;
                break;
            default:
                animation = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, animation, 1, 0, 4);
    }
    SceneActor_SetFlags(p, 0x1000);
    if ((u8)Callback_InvokeActor(p, p->query) != 1 && sub_081C8FB0(context, p, 0)) {
        SceneActor_SetFlags(p, 0x800);
    }
}
