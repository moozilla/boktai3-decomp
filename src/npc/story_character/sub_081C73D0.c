#include "npc/manager/scene_actor_lifecycle.h"
extern const s16 gUnk_086149C4[];
static inline s32 shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
void StoryCharacter_UpdateAnimation13State(u32 a, struct SceneActor *p)
{
    u32 r;
    u32 speed;

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
                r = 0;
                break;
            case 27:
                r = 13;
                break;
            case 28:
            case 29:
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 1, 4);
    }

    speed = p->distance;
    if (speed == 0) {
        p->finished = 1;
        return;
    }
    {
        struct ContactPosition *vel = &p->body.motion;
        u8 angle = p->direction;
        u32 spd = speed;
        vel->x = shift12(spd * gUnk_086149C4[(angle + 0x40) & 0xff]);
        vel->y = 0;
        vel->z = shift12(spd * gUnk_086149C4[angle]);
    }
    p->distance = (u32)p->distance * 3 / 4;
}
