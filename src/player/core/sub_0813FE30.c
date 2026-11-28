#include "player/appearance/player_visuals.h"

// Initializes both animation channels and the script-selected facing.

struct AnimationHeader {
    u32 words[8];
};

void sub_0813FE30(struct Player *p)
{
    struct AnimationHeader *source = ScriptProc_Start(0xCB05, 0xDE23);
    if (source) {
        *((struct AnimationHeader *)p->animationData) = *source;
        ResourceHeader_Relocate(p->animationData, source);
        AnimSprite_InitLinked(&p->animation, p->animationData, 0, 0x9000, 2, 0, 60, &p->position);
        AnimSprite_InitLinked(&p->secondaryAnimation, p->animationData, 0, 0x1001, 2, 0, 60,
                              &p->position);
        if (Script_SeekToKeyword('d'))
            p->facing = Script_GetValue();
        else
            p->facing = 0;
        if (p->actionMode == 3) {
            p->facing |= 1;
            if (p->facing > 4) {
                p->heading = (8 - p->facing) >> 1;
                p->side = 1;
            } else {
                p->heading = p->facing >> 1;
                p->side = 0;
            }
        } else {
            if (p->facing > 4) {
                p->heading = 8 - p->facing;
                p->side = 1;
            } else {
                p->heading = p->facing;
                p->side = 0;
            }
        }
        p->animationIndex = 0;
        Anim_SetKeyframes(&p->animation, p->animationData, p->heading, 1, 0);
        Anim_SetKeyframes(&p->secondaryAnimation, p->animationData,
                          (u16)(p->animationIndex + p->heading), 1, 0);
        if (p->side) {
            (&p->animation)->flags |= 4;
            (&p->secondaryAnimation)->flags |= 4;
        } else {
            (&p->animation)->flags &= ~4;
            (&p->secondaryAnimation)->flags &= ~4;
        }
    }
}
