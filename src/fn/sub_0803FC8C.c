#include "modifier_list.h"
s32 ScriptRecord_FindAttributeModifier(struct ModifierItemInfo *, s32);
void Modifier_LoadFourthSource(struct ModifierSelection *selection)
{
    struct ModifierItemInfo info;
    if (ScriptRecord_FindAttributeModifier(&info, selection->sourceIds[3]) < 0) {
        selection->valuesE[3] = 0xffff;
        selection->valuesE[4] = -1;
    } else {
        selection->valuesE[3] = info.value6;
        selection->valuesE[4] = info.value4;
    }
}
