#include "global.h"
extern const u32 gUnk_08606000[];
void Vorn_InitReactionActionTable(u8 *p)
{
    *(const u32 **)(p + 0x28c) = gUnk_08606000;
}
