#include "player_3.h"

#ifdef NONMATCHING
// fuzzy: 83.1%
// Initializes both animation channels and script facing; constructor allocation differs.

struct AnimationHeader {
    u32 words[8];
};
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);

void sub_0813FE30(struct Player *p)
{
    struct AnimationHeader *source = ScriptProc_Start(0xCB05, 0xDE23);
    void *header;
    struct PlayerAnimation *primary, *secondary;
    if (source) {
        *(struct AnimationHeader *)p->animationData = *source;
        ResourceHeader_Relocate(p->animationData, source);
        header = p->animationData;
        primary = &p->animation;
        Widget_InitLinked(primary, header, 0, 0x9000, 2, 0, 60, &p->position);
        secondary = &p->secondaryAnimation;
        Widget_InitLinked(secondary, header, 0, 0x1001, 2, 0, 60, &p->position);
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
            u32 direction = p->facing;
            if (direction > 4) {
                u32 side = 8 - direction;
                p->heading = side;
                p->side = 1;
            } else {
                p->heading = direction;
                p->side = 0;
            }
        }
        p->animationIndex = 0;
        Anim_SetKeyframes(primary, header, p->heading, 1, 0);
        Anim_SetKeyframes(secondary, header, (u16)(p->animationIndex + p->heading), 1, 0);
        if (p->side) {
            primary->flags |= 4;
            secondary->flags |= 4;
        } else {
            primary->flags &= ~4;
            secondary->flags &= ~4;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0813FE30);
#endif
