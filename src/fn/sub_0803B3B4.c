#include "global.h"
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
extern struct ResultMenu *gUnk_02000104;
extern void *gUnk_08605814[];
extern u16 gUnk_03005260[], gUnk_03004BD8;
void sub_0822AFE8(int);
void Sound_PlaySong(int);
void sub_08249238(void *);
void Task_Kill(void *);
int sub_08036BEC(void);
void sub_08033778(int);
void sub_0824923C(void *, void *);
void Anim_Remove(void *);
void Sprite_Remove(void *);
int sub_0803B3B4(struct ResultMenu *p)
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
        sub_08033778(*w++);
    } while (--i >= 0);
    {
        u16 *shadow = &gUnk_03004BD8;
        u32 mask;
        mask = ~0xC00;
        mask &= *shadow;
        *shadow = mask;
    }
    gUnk_02000104 = 0;
    return 0;
}