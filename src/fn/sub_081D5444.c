#include "global.h"
// Family and descriptive names are tentative interpretations of static code.
#define BackgroundPanel_ReleaseHandles sub_081D5444
struct BackgroundPanel {
    u8 pad0[0x28];
    u32 handles[8];
    u8 pad48[0xd06];
    u16 flags;
};
void TextWindow_Clear(u32);
void Bg_ClearTilemap(s32);
void sub_081D5444(struct BackgroundPanel *p)
{
    u32 *handle = p->handles;
    s32 i;
    for (i = 7; i >= 0; i--) TextWindow_Clear(*handle++);
    p->flags |= 1;
    Bg_ClearTilemap(0);
}
