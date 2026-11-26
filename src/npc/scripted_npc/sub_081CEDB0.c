#include "global.h"

extern u32 gUnk_086124D4[];
void sub_08249240(u32, u8 *, u32);

void SceneNpc_DispatchModeHandler(u32 a, u8 *b)
{
    sub_08249240(a, b, gUnk_086124D4[b[4]]);
}
