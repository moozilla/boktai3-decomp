#include "global.h"
/* TextWindow_SetNumber: Stores a value in the embedded interpreter numeric substitution array */
#define TextWindow_SetNumber sub_08033A10

u8 *TextWindow_Find(u32);
void TextState_SetNumber(void *, u32, u32);

s32 sub_08033A10(u32 id, u32 a, u32 b)
{
    u8 *w = TextWindow_Find(id);
    if (w == NULL)
        return -1;
    TextState_SetNumber(w + 0x60, a, b);
    return 0;
}
