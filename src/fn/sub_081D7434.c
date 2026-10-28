#include "global.h"
#include "menu_parameters.h"

extern u16 gUnk_03005260[];

struct ScriptArgs {
    u32 count : 16;
    u32 rest : 16;
    s32 *vals;
};

struct OptionMenu {
    u8 pad[0x18];
    s32 sel;
    s32 timer;
};

void Sound_PlaySong(s32);
void Task_Kill(void *);
void Script_Call(u32, void *);
s32 sub_08249350(s32, s32);

void OptionMenu_Update(struct OptionMenu *p)
{
    s32 zero = 0;
    struct ScriptArgs args;
    struct ScriptArgs *ap;
    args.count = 1;
    args.vals = &zero;
    if (gUnk_03005260[1] & 0x40) {
        Sound_PlaySong(0xDC);
        p->sel--;
        if (p->sel < 0)
            p->sel = 7;
    }
    ap = &args;
    if (gUnk_03005260[1] & 0x80) {
        Sound_PlaySong(0xDC);
        p->sel++;
        if (p->sel > 7)
            p->sel = 0;
    }
    if (gUnk_03005260[1] & 1) {
        if (p->sel == 1)
            Script_Call(gOptionMenuParameters.callbacks.entries[1], ap);
        else
            Script_Call(gOptionMenuParameters.callbacks.entries[p->sel], 0);
        Task_Kill(p);
    } else {
        p->timer++;
        if (sub_08249350(p->timer, 600) == 0) {
            Script_Call(gOptionMenuParameters.callbacks.entries[6], 0);
            Task_Kill(p);
        }
    }
}
