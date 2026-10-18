#define MessagePanel_ResetDisplay sub_08109948
#define MessagePanel_SetCallbacks LinkConnection_SetCallbacks
#include "global.h"
#include "engine.h"
struct MessagePanel {
    u8 unk0[0x20];
    void (*state)(struct MessagePanel *);
    const u8 *script;
    u8 unk28[8];
    s32 window;
    u8 mode;
    u8 entering;
    u16 timer;
    u32 unk38;
    u32 unk3C;
};
void sub_08108FB4(s32,s32,s32,s32,u32);
void sub_0810963C(void);
void sub_081095B4(void);
void LinkConnection_SetCallbacks(void *, void (*)(void), void (*)(void));
void sub_08109948(struct MessagePanel *p)
{
    if (p->entering == 1) {
        BgGfx_LoadTilemapSimple(3,0,0,0,0);
        p->entering = 0;
        sub_08108FB4(-1,1,0,0x36,p->unk3C);
        LinkConnection_SetCallbacks(p,sub_0810963C,sub_081095B4);
        p->timer = 0;
    }
    if (p->timer < 30) p->timer++;
}
