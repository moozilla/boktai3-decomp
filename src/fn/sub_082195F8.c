#include "global.h"
/* Anim_RemoveAllInCurrentList: Walks the current animation list and removes every entry while preserving the next pointer */
#define Anim_RemoveAllInCurrentList sub_082195F8

extern u8 *gUnk_030042C8[];
extern u32 gUnk_03004290;
void Anim_Remove(void *);

void sub_082195F8(void)
{
    u8 *a = gUnk_030042C8[gUnk_03004290];
    while (a) {
        u8 *next = *(u8 **)(a + 0x5c);
        Anim_Remove(a);
        a = next;
    }
}
