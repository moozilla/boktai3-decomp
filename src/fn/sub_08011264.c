#include "global.h"
struct Menu {
    u8 pad[0x3c];
    u8 lines[4][256];
    u32 pad43c;
    void *text;
    int mainText, footerText, choiceText;
};
int sub_08033690(int, int, int, int);
void sub_08033A38(int, int, void *);
void sub_0803386C(int, void *);
void sub_08033924(int, int);
void sub_080337FC(int);
void *sub_0821A520(int, int);
extern u16 gUnk_030051A0[];
int MessageMenu_CreateTextWindows(struct Menu *m)
{
    int *handle = &m->mainText;
    int i;
    u8 *palette;
    *handle = sub_08033690(2, 5, 28, 4);
    if (*handle < 0)
        return -1;
    for (i = 0; i < 4; i++)
        sub_08033A38(*handle, i, m->lines[i]);
    sub_0803386C(*handle, m->text);
    sub_08033924(*handle, 0);
    sub_080337FC(*handle);
    handle = &m->footerText;
    *handle = sub_08033690(6, 17, 28, 2);
    if (*handle < 0)
        return -1;
    sub_0803386C(*handle, m->text);
    sub_08033924(*handle, 1);
    sub_080337FC(*handle);
    handle = &m->choiceText;
    *handle = sub_08033690(0, 11, 30, 4);
    if (*handle < 0)
        return -1;
    sub_0803386C(*handle, m->text);
    sub_08033924(*handle, 2);
    sub_080337FC(*handle);
    palette = sub_0821A520(0x92B3, 0x519C);
    if (!palette)
        return -1;
    CpuSet(palette + 500, gUnk_030051A0, 0x04000008);
    return 0;
}
