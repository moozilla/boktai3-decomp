#include "modifier_list.h"
u32 Script_GetTextSourcePc(void);
void ModifierMenu_ReadTextSource(struct ModifierMenu *menu, u32 index)
{
    menu->textSources[index] = (const u8 *)Script_GetTextSourcePc();
}
