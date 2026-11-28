#include "global.h"
#include "npc/manager/scene_actor_lifecycle.h"
s32 Callback_InvokeActor(void *, void *);
s32 sub_081C8FB0(void *, void *, s32);
void StoryCharacter_StartAnimStep(void *a, struct SceneActor *p)
{
    u32 r;
    u8 hit;
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
                r = 0;
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
                r = 0;
                break;
            case 29:
                r = 0xb;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
    }
    SceneActor_SetFlags(p, 0x1000);
    hit = 0;
    if ((u8)Callback_InvokeActor(p, p->query) != 1 && sub_081C8FB0(a, p, 0)) {
        hit = 1;
        SceneActor_SetFlags(p, 0x800);
    }
}
