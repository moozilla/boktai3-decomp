#include "global.h"
#include "render.h"
#include "ram.h"
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
extern u16 gUnk_03005260[];
void Sound_PauseChannel10(int);
void Sound_PlaySong(int);
void sub_08249238(void *);
void Task_Kill(void *);
int Wireless_GetStatus(void);
void TextWindow_Destroy(int);
void Callback_InvokeActor(void *, void *);
void Anim_Remove(void *);
void Sprite_Remove(void *);
void WirelessResultPanel_UpdateDismissal(struct ResultMenu *p)
{
    if (p->entered)
        p->entered = 0;
    if (p->timer > 29 && (gUnk_03005260[1] & 0x30F)) {
        Sound_PauseChannel10(4);
        Sound_PlaySong(221);
        if (p->onClose)
            sub_08249238(p->onClose);
        Task_Kill(p);
    } else
        p->timer++;
}