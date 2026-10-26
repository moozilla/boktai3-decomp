#include "global.h"
#include "ram.h"
void ForgingBook_RemoveDisplayObjects(u32);
u32 ForgingBook_ReleaseBackgrounds(u32);
u32 ForgingBook_OnTaskDestroy(u32 a)
{
    ForgingBook_RemoveDisplayObjects(a);
    ForgingBook_ReleaseBackgrounds(a);
    gForgingBook = 0;
}
