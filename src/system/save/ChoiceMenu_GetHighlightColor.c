#include "global.h"
extern const u8 gUnk_08E87968[];
extern const u8 gUnk_08E8796E[];
u8 ChoiceMenu_GetHighlightColor(s32 value)
{
    s32 limit = gUnk_08E87968[0];
    s32 i;

    for (i = 0; i <= 5; i++) {
        if (value <= limit)
            return gUnk_08E8796E[i];
        limit += gUnk_08E87968[i];
    }
    return gUnk_08E8796E[0];
}
