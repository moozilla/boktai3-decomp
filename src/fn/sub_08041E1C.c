#include "modifier_list.h"
#define ModifierMenu_LoadTextSources sub_08041E1C
void sub_08041E04(struct ModifierMenu *, u32);
void ModifierMenu_LoadTextSources(struct ModifierMenu *menu)
{
    if (Script_SeekToKeyword('G'))
        sub_08041E04(menu, 0);
    if (Script_SeekToKeyword('F'))
        sub_08041E04(menu, 1);
    if (Script_SeekToKeyword('B'))
        sub_08041E04(menu, 2);
    if (Script_SeekToKeyword('T'))
        sub_08041E04(menu, 3);
    if (Script_SeekToKeyword('S'))
        sub_08041E04(menu, 4);
    if (Script_SeekToKeyword('O'))
        sub_08041E04(menu, 5);
    if (Script_SeekToKeyword('C'))
        sub_08041E04(menu, 6);
    if (Script_SeekToKeyword('f'))
        sub_08041E04(menu, 7);
    if (Script_SeekToKeyword('b'))
        sub_08041E04(menu, 8);
    if (Script_SeekToKeyword('t'))
        sub_08041E04(menu, 9);
    if (Script_SeekToKeyword('s'))
        sub_08041E04(menu, 10);
    if (Script_SeekToKeyword('o'))
        sub_08041E04(menu, 11);
    if (Script_SeekToKeyword('c'))
        sub_08041E04(menu, 12);
}
