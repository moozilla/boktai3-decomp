#include "npc/story_character/scene_character.h"

s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
u8 *TimedSpriteEffect_Create(u8 *pos, u32 flag, s32 duration, s32 arg3, s32 arg4, s32 arg5,
                             s32 arg6, s32 arg7, u32 arg8);

struct VecCB3F4 {
    s16 x, y, z, pad;
};

void StoryCharacter_UpdateKind29AttackEffect(u32 a, struct SceneNpc *actor, u32 c)
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
                anim = 4;
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
            if (actor->stateFrame > 0x3c) {
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
                        anim = 5;
                        break;
                    default:
                        anim = 0;
                        break;
                }
                Actor_PlayAnimationEntry(actor, anim, 2, 1, 4);
                actor->animationPhase++;
            }
            break;
        case 1:
            if (DisplayAnimation_TestFinished(&actor->display)) {
                const s16 *coords = (const s16 *)&actor->body.position;
                s32 x = *coords++;
                s32 y = *coords++;
                s32 z = *coords;
                struct VecCB3F4 v;
                s16 *q = (s16 *)&v;
                s32 vx = x - 0x20;
                s32 vy = y + 0x180;
                s32 vz = z + 0x20;
                v.x = vx;
                q[1] = vy;
                q[2] = vz;
                TimedSpriteEffect_Create((u8 *)&v, 0xa0, 0x20, 1, 6, 0, 0, 0, 0);
                actor->animationPhase++;
            }
            break;
        case 2:
            actor->done = 1;
            break;
    }
}
