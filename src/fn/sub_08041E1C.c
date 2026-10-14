#include "modifier_list.h"
void ModifierMenu_ReadTextSource(struct ModifierMenu *, u32);
void ModifierMenu_LoadTextSources(struct ModifierMenu *menu)
{
    if (Script_SeekToKeyword('G'))
        ModifierMenu_ReadTextSource(menu, 0);
    if (Script_SeekToKeyword('F'))
        ModifierMenu_ReadTextSource(menu, 1);
    if (Script_SeekToKeyword('B'))
        ModifierMenu_ReadTextSource(menu, 2);
    if (Script_SeekToKeyword('T'))
        ModifierMenu_ReadTextSource(menu, 3);
    if (Script_SeekToKeyword('S'))
        ModifierMenu_ReadTextSource(menu, 4);
    if (Script_SeekToKeyword('O'))
        ModifierMenu_ReadTextSource(menu, 5);
    if (Script_SeekToKeyword('C'))
        ModifierMenu_ReadTextSource(menu, 6);
    if (Script_SeekToKeyword('f'))
        ModifierMenu_ReadTextSource(menu, 7);
    if (Script_SeekToKeyword('b'))
        ModifierMenu_ReadTextSource(menu, 8);
    if (Script_SeekToKeyword('t'))
        ModifierMenu_ReadTextSource(menu, 9);
    if (Script_SeekToKeyword('s'))
        ModifierMenu_ReadTextSource(menu, 10);
    if (Script_SeekToKeyword('o'))
        ModifierMenu_ReadTextSource(menu, 11);
    if (Script_SeekToKeyword('c'))
        ModifierMenu_ReadTextSource(menu, 12);
}
