#include "modifier_list.h"
s32 sub_0811E534(struct ModifierItemInfo *, s32);
void Modifier_LoadFourthSource(struct ModifierSelection *selection)
{
    struct ModifierItemInfo info;
    if (sub_0811E534(&info, selection->sourceIds[3]) < 0) {
        selection->valuesE[3] = 0xffff;
        selection->valuesE[4] = -1;
    } else {
        selection->valuesE[3] = info.value6;
        selection->valuesE[4] = info.value4;
    }
}
