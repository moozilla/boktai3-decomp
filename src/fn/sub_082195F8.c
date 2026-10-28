#include "global.h"
#include "render.h"
/* Anim_RemoveAllInCurrentList: Walks the current animation list and removes every entry while preserving the next pointer */
#define Anim_RemoveAllInCurrentList sub_082195F8

void Anim_Remove(void *);

void sub_082195F8(void)
{
    u8 *a = gMainSpriteListHeads[gRenderListIndex];
    while (a) {
        u8 *next = *(u8 **)(a + 0x5c);
        Anim_Remove(a);
        a = next;
    }
}
