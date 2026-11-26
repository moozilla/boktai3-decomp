#include "global.h"
/* TextWindow_SetNumber: Stores a value in the embedded interpreter numeric substitution array */

u8 *TextWindow_Find(u32);
void TextState_SetNumber(void *, u32, u32);

s32 TextWindow_SetNumber(u32 id, u32 a, u32 b)
{
    u8 *w = TextWindow_Find(id);
    if (w == NULL)
        return -1;
    TextState_SetNumber(w + 0x60, a, b);
    return 0;
}
