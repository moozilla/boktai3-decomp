#include "global.h"
#include "render.h"
#include "ram.h"
#include "input.h"
// Tentative result-menu offsets from the contiguous callback family.
struct Drawing {
    u8 bytes[0x60];
};
struct ResultMenu {
    u8 task[0x18];
    u32 timer;
    u8 state, entered;
    u8 pad1e[0x9a];
    struct Drawing draws[22];
    void *onClose, *resource;
    int windows[4];
};
extern void *gUnk_08605814[];
void Sound_PauseChannel10(int);
void Sound_PlaySong(int);
void sub_08249238(void *);
void Task_Kill(void *);
int Wireless_GetStatus(void);
void TextWindow_Destroy(int);
void Callback_InvokeActor(void *, void *);
void Anim_Remove(void *);
void Sprite_Remove(void *);
int WirelessResultPanel_Destroy(struct ResultMenu *p)
{
    struct Drawing *d = p->draws;
    int i = 21;
    int *w;
    do {
        Anim_Remove(d);
        i--;
        d++;
    } while (i >= 0);
    Sprite_Remove((u8 *)p + 0x70);
    w = p->windows;
    i = 3;
    do {
        TextWindow_Destroy(*w++);
    } while (--i >= 0);
    {
        u16 *shadow = &gDispCntShadow;
        u32 mask;
        mask = ~0xC00;
        mask &= *shadow;
        *shadow = mask;
    }
    gResultMenu = 0;
    return 0;
}