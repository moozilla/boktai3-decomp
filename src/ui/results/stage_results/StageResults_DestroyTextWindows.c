#include "global.h"

struct StageResults {
    u8 pad0[0x11CC];
    u32 textWindows[3];
};

void TextWindow_Clear(u32);

void StageResults_DestroyTextWindows(struct StageResults *results)
{
    s32 i;
    for (i = 0; i < 3; i++)
        TextWindow_Clear(results->textWindows[i]);
}
