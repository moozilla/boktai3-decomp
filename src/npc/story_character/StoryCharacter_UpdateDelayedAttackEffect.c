#include "npc/story_character/scene_character.h"
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
void TimedSpriteEffect_Create(struct ContactPosition *, s32, s32, s32, s32, s32, s32, s32, s32);
/* Three-phase attack: play the wind-up, spawn an effect above the actor on
 * frame 15, then the follow-through after 0x56 frames. */
void StoryCharacter_UpdateDelayedAttackEffect(u8 *context, struct SceneNpc *actor, s32 frame)
{
    u32 anim;
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->done = 0;
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
                anim = 0;
                break;
            case 16:
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
                anim = 8;
                break;

            default:
                anim = 0;
                break;
        }

        Actor_PlayAnimationEntry(actor, anim, 2, 1, 4);
        actor->animationPhase = 0;
        actor->stateFrame = 0;
    }
    switch (actor->animationPhase) {
        case 0:
            if (frame == 15) {
                s32 x = actor->body.position.x;
                s32 y = actor->body.position.y;
                s32 z = actor->body.position.z;
                struct ContactPosition position;
                struct ContactPosition *out = &position;
                x -= 0x20;
                y += 0x180;
                z += 0x20;
                out->x = x;
                out->y = y;
                out->z = z;
                TimedSpriteEffect_Create(&position, 0xA0, 0x20, 1, 1, 0, 0, 0, 0);
            }
            if (DisplayAnimation_TestFinished(&actor->display))
                actor->animationPhase = 1;
            break;
        case 1:
            if ((actor->stateFrame) > 0x56) {
                actor->animationPhase = 2;
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
                        anim = 0;
                        break;
                    case 16:
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
                        anim = 9;
                        break;

                    default:
                        anim = 0;
                        break;
                }

                Actor_PlayAnimationEntry(actor, anim, 2, 1, 4);
            }
            break;
        case 2:
            if (DisplayAnimation_TestFinished(&actor->display))
                actor->done = 1;
            break;
    }
}
