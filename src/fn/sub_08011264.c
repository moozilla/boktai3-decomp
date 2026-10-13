#include "message_menu.h"
int MessageMenu_CreateTextWindows(struct MessageMenu *m)
{
    int *handle = &m->mainText;
    int i;
    u8 *palette;
    *handle = sub_08033690(2, 5, 28, 4);
    if (*handle < 0)
        return -1;
    for (i = 0; i < 4; i++)
        sub_08033A38(*handle, i, m->lines[i]);
    TextWindow_SetScript(*handle, m->text);
    TextWindow_Refresh(*handle, 0);
    sub_080337FC(*handle);
    handle = &m->footerText;
    *handle = sub_08033690(6, 17, 28, 2);
    if (*handle < 0)
        return -1;
    TextWindow_SetScript(*handle, m->text);
    TextWindow_Refresh(*handle, 1);
    sub_080337FC(*handle);
    handle = &m->choiceText;
    *handle = sub_08033690(0, 11, 30, 4);
    if (*handle < 0)
        return -1;
    TextWindow_SetScript(*handle, m->text);
    TextWindow_Refresh(*handle, 2);
    sub_080337FC(*handle);
    palette = ScriptProc_Start(0x92B3, 0x519C);
    if (!palette)
        return -1;
    CpuSet(palette + 500, gUnk_030051A0, 0x04000008);
    return 0;
}
