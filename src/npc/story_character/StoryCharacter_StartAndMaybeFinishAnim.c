#include "npc/manager/scene_actor_lifecycle.h"
#include "npc/story_character/npc_tag_layout.h"
void StoryCharacter_StartAndMaybeFinishAnim(u32 a, struct SceneActor *p, s32 a2)
{
    struct NpcTag *tag = (struct NpcTag *)p->unknown20;
    if (p->animationEntered != 0) {
        u32 animId;
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
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                animId = 0;
                break;
            case 27:
                animId = 0xe;
                break;
            default:
                animId = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, animId, 2, 0, 6);
        tag->height = 300;
        p->animationPhase = 0;
    }
    if (a2 != 1)
        return;
    {
        u32 animId;
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
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                animId = 0;
                break;
            case 27:
                animId = 0xe;
                break;
            default:
                animId = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, animId, 0, 0, 3);
    }
    p->finished = 1;
}
