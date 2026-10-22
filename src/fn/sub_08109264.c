#include "global.h"
#include "engine.h"
struct StackPoint {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
s32 LinkSession_ClearFirstPlayerKind(void *, s32);
void SelectionSlots_ClearValue(void *, s32);
s32 sub_08109264(void)
{
    struct StackPoint pos;
    s32 slot;
    if (Script_SeekToKeyword('p')) {
        pos.x = Script_GetValue();
        pos.y = Script_GetValue();
        pos.z = Script_GetValue();
    } else {
        pos.x = 0;
        pos.y = 0;
        pos.z = 0;
    }
    slot = Script_GetKeywordValue('t', 4);
    if (!Script_GetKeywordValue('m', 0))
        LinkSession_ClearFirstPlayerKind(&pos, slot);
    else
        SelectionSlots_ClearValue(&pos, slot);
}