#include "global.h"
#include "ui/save/save_menu.h"

void sub_08110164(struct SaveMenu *menu)
{
    if (menu->unknown1028) {
        Script_Call(menu->unknown1028, 0);
        Task_Kill(menu);
    }
}
