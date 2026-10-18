#include "global.h"

void Collection_LoadPageBackground(void *);
void Collection_UpdateEntryVisibility(void *);
void SceneKeyframe_SetGridPosition(void *);
void Collection_UpdateCountDigits(void *);
void Collection_UpdatePageIndicator(void *);
void Collection_UpdateSelectedEntryText(void *);

s32 Collection_RefreshPage(void *s)
{
    Collection_LoadPageBackground(s);
    Collection_UpdateEntryVisibility(s);
    SceneKeyframe_SetGridPosition(s);
    Collection_UpdateCountDigits(s);
    Collection_UpdatePageIndicator(s);
    Collection_UpdateSelectedEntryText(s);
    return 0;
}
