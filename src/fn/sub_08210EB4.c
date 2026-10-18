#include "global.h"
void SceneKeyframe_SetGridPosition(u32);
void Collection_UpdateCountDigits(u32);
void Collection_UpdateSelectedEntryText(u32);
u32 Collection_RefreshSelection(u32 a)
{
    SceneKeyframe_SetGridPosition(a);
    Collection_UpdateCountDigits(a);
    Collection_UpdateSelectedEntryText(a);
    return 0;
}
