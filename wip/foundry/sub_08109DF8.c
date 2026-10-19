#define ScriptPanel_PollSelection sub_08109DF8
#include "global.h"
struct ScriptPanel {
    u8 unk0[0x1D];
    u8 mode;
    u8 unk1E[0x10];
    u16 retryTimer;
    u32 counter;
    u8 unk34[0xA];
    u16 scale;
    u8 unk40[0x24];
    u8 buttons[4];
    u16 selections[2];
};
u8 sub_08138168(struct ScriptPanel *);
void sub_08138154(struct ScriptPanel *, u32, void *);
void sub_082232A0(void);
s32 sub_0822323C(void);
void sub_08223270(void);
void sub_08109E98(void);
s32 sub_08109DF8(struct ScriptPanel *p)
{
    struct ScriptPanel *q = p;
    if (sub_08138168(p)) {
        s32 i;
        u8 *buttons;
        u32 zero;
        u32 mask, savedMask;
        u16 *selections;
        sub_082232A0();
        i = 0;
        buttons = p->buttons;
        zero = 0;
        mask = 0xFFFF;
        savedMask = mask;
        selections = p->selections;
        do {
            buttons[i] = zero;
            *selections = savedMask | *selections;
            selections++;
            i++;
        } while (i <= 1);
        p->scale = 0;
        p->retryTimer = 0;
    } else if (p->counter > 19) {
        s32 mode = sub_0822323C();
        if (mode >= 0) {
            p->mode = mode;
            sub_08138154(p, 3, sub_08109E98);
        } else {
            q->retryTimer++;
            if (q->retryTimer > 29) {
                u8 *base, *button;
                u32 zero;
                sub_08138154(q, 2, sub_08109DF8);
                sub_08223270();
                base = q->buttons;
                zero = 0;
                button = &q->buttons[1];
                do {
                    *button-- = zero;
                } while ((s32)button >= (s32)base);
            }
        }
    }
    return 0;
}
