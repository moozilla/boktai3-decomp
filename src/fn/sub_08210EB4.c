#include "global.h"
void sub_08210C14(u32);
void Collection_UpdateCountDigits(u32);
void Collection_UpdateSelectedEntryText(u32);
u32 Collection_RefreshSelection(u32 a)
{
    sub_08210C14(a);
    Collection_UpdateCountDigits(a);
    Collection_UpdateSelectedEntryText(a);
    return 0;
}
