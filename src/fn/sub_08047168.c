#include "damage_number_display.h"
#define DamageNumberDisplay_Update       sub_08047168
#define DamageNumberDisplay_GetActiveBit sub_08046B4C
u32 DamageNumberDisplay_GetActiveBit(struct DamageNumberDisplay *, u32);
s32 sub_08249244(void *, void *, u32, void *);
extern void (*const gUnk_086059D4[])(struct DamageNumberDisplay *, struct DamageNumberGroup *, u32);
s32 DamageNumberDisplay_Update(struct DamageNumberDisplay *display)
{
    struct DamageNumberGroup *group = display->groups;
    s32 index = 0;
    do {
        if (DamageNumberDisplay_GetActiveBit(display, index))
            sub_08249244(display, group, index, (void *)gUnk_086059D4[group->phase]);
        ++index;
        ++group;
    } while (index <= 3);
    return 0;
}
