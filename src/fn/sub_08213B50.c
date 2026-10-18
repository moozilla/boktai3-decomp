#include "global.h"
void ForgingBook_ReleaseGraphics(u32);
u32 ForgingBook_FinalizeState(u32);
u32 sub_08213B50(u32 a)
{
    ForgingBook_ReleaseGraphics(a);
    ForgingBook_FinalizeState(a);
}
