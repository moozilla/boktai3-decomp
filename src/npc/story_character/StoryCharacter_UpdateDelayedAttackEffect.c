#include "npc/story_character/scene_character.h"
struct Vec081CBE30 {
    s16 x, y, z, pad;
};
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
void TimedSpriteEffect_Create(struct Vec081CBE30 *, s32, s32, s32, s32, s32, s32, s32, s32);
/* Three-phase attack: play the wind-up, spawn an effect above the actor on
 * frame 15, then the follow-through after 0x56 frames. */
void StoryCharacter_UpdateDelayedAttackEffect(u8 *a, struct SceneNpc *actor, s32 inputFrame)
{
    s32 frame = inputFrame;
    u32 anim;
    s32 state;
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
    state = actor->animationPhase;
    switch (state) {
        case 0:
            if (frame == 15) {
                const s16 *coords = (const s16 *)&actor->body.position;
                s32 x = *coords++;
                s32 y = *coords++;
                s32 z = *coords;
                struct Vec081CBE30 v;
                s16 *q = (s16 *)&v;
                s32 vx = x - 0x20;
                s32 vy = y + 0x180;
                s32 vz = z + 0x20;
                v.x = vx;
                q[1] = vy;
                q[2] = vz;
                TimedSpriteEffect_Create(&v, 0xA0, 0x20, 1, 1, state, state, state, state);
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
