#include "global.h"

void Collection_LoadPageBackground(void *);
void sub_08210AD4(void *);
void sub_08210C14(void *);
void Collection_UpdateCountDigits(void *);
void Collection_UpdatePageIndicator(void *);
void Collection_UpdateSelectedEntryText(void *);

s32 Collection_RefreshPage(void *s)
{
    Collection_LoadPageBackground(s);
    sub_08210AD4(s);
    sub_08210C14(s);
    Collection_UpdateCountDigits(s);
    Collection_UpdatePageIndicator(s);
    Collection_UpdateSelectedEntryText(s);
    return 0;
}
