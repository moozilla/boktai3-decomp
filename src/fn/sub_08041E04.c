#include "modifier_list.h"
u32 sub_08227E90(void);
void ModifierMenu_ReadTextSource(struct ModifierMenu *menu, u32 index)
{
    menu->textSources[index] = (const u8 *)sub_08227E90();
}
