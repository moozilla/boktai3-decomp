#include "global.h"
struct Panel {
    u8 bytes[0x25c];
};
struct P {
    u8 pad[0x2c];
    struct Panel panels[2];
    u32 windows[3];
    u8 pad1[0x144];
    u8 draw[1];
};
void WirelessPeerPanel_Destroy(struct P *, struct Panel *, int);
void TextWindow_Destroy(u32);
void Anim_Remove(void *);
int sub_08037B6C(struct P *p)
{
    struct Panel *q = p->panels;
    int i = 0;
    u32 *w;
    do {
        WirelessPeerPanel_Destroy(p, q, i);
        i++;
        q++;
    } while (i <= 1);
    w = p->windows;
    i = 2;
    do {
        TextWindow_Destroy(*w++);
        i--;
    } while (i >= 0);
    Anim_Remove(p->draw);
    return 0;
}
