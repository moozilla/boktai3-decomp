#include "npc/story_character/scene_character.h"

s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);

void StoryCharacter_UpdateFacingAnimation(u32 a, struct SceneNpc *actor, u32 c)
{
    u32 anim;
    struct SceneCharacterEffects *effects = &actor->effects;
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->done = 0;
        if (effects->tag == 1) {
            switch (actor->kind) {
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
                    anim = 0;
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
                    anim = 0;
                    break;
                case 29:
                    anim = 6;
                    break;
                default:
                    anim = 0;
                    break;
            }
            Actor_PlayAnimationEntry(actor, anim, 1, 1, 4);
        } else {
            switch (actor->kind) {
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
                    anim = 0;
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
                    anim = 0;
                    break;
                case 29:
                    anim = 0xd;
                    break;
                default:
                    anim = 0;
                    break;
            }
            Actor_PlayAnimationEntry(actor, anim, 0, 1, 4);
        }
        actor->animationPhase = 0;
        actor->stateFrame = 0;
    } else {
        if (effects->tag == 1) {
            switch (actor->kind) {
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
                    anim = 0;
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
                    anim = 0;
                    break;
                case 29:
                    anim = 6;
                    break;
                default:
                    anim = 0;
                    break;
            }
            Actor_PlayAnimationEntry(actor, anim, 1, 1, 1);
        } else {
            switch (actor->kind) {
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
                    anim = 0;
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
                    anim = 0;
                    break;
                case 29:
                    anim = 0xd;
                    break;
                default:
                    anim = 0;
                    break;
            }
            Actor_PlayAnimationEntry(actor, anim, 0, 1, 1);
        }
    }
    if (DisplayAnimation_TestFinished(&actor->display))
        actor->done = 1;
}
