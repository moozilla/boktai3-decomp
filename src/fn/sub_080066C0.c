#include "choice_menu.h"

s32 sub_08031E34(void *);
u32 sub_082282A4(u32);
u32 sub_080066C0(struct ChoiceMenu *p)
{
    if (p->window != 0 && sub_08031E34(p->window) != 0) {
        if (p->delay == 0) {
            if (ChoiceMenu_HandleInput(p) == 1)
                return 0;
        } else {
            p->delay--;
        }
        p->palette[12] = sub_082282A4(p->colorIndex);
        p->colorIndex = (p->colorIndex + 1) & 0x3f;
    }
    return 0;
}
