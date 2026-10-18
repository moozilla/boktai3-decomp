#include "global.h"
void ForgingBook_RemoveDisplayObjects(u32);
u32 ForgingBook_ReleaseBackgrounds(u32);
extern u32 gUnk_02000608;
u32 ForgingBook_OnTaskDestroy(u32 a)
{
    ForgingBook_RemoveDisplayObjects(a);
    ForgingBook_ReleaseBackgrounds(a);
    gUnk_02000608 = 0;
}
