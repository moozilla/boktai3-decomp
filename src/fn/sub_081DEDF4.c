#include "global.h"
void Anim_Remove(void *);
/* Tentative panel family: four main animations and twelve small entries. */
struct AnimPanel {
    u8 task[0x18];
    u8 mainAnimations[4][0x88];
    u8 entryAnimations[12][0x60];
};
void AnimPanel_RemoveAnimations(struct AnimPanel *p)
{
    u8 *entry = p->entryAnimations[0];
    s32 i = 11;
    do {
        Anim_Remove(entry);
        entry += 0x60;
    } while (--i >= 0);
    Anim_Remove(p->mainAnimations[3]);
    Anim_Remove(p->mainAnimations[2]);
    Anim_Remove(p->mainAnimations[1]);
    Anim_Remove(p->mainAnimations[0]);
}
