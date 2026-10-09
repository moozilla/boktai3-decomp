#include "global.h"
struct Task { u8 pad[24]; s32 state, counter; };
extern u32 gUnk_03005408, gUnk_030054B0, gUnk_03003A08, gUnk_030053E8, gUnk_030053FC;
extern u32 gUnk_0300523C, gUnk_030053DC, gUnk_03005414, gUnk_030051D8, gUnk_0300540C;
extern u32 gUnk_030053F4, gUnk_03005404, gUnk_030053E4, gUnk_030053D8;
extern u16 gUnk_03005260, gUnk_030039F0, gUnk_03004BD8;
extern u8 *gUnk_02000710;
extern u32 *gUnk_030053F8;
void sub_08224FA4(void);
void sub_0822502C(void);
void sub_0821B078(void);
u32 sub_0821AD08(u32, u32);
void sub_0821B004(void);
s32 sub_08224F2C(void);
void sub_0821A0F8(u32);
void sub_082250D8(void);
void sub_082250DC(void);
void sub_0821B148(void);
void sub_0821B180(void);
void sub_0821B03C(s32);
void sub_08228928(void);
s32 sub_082250FC(struct Task *task)
{
    if (!gUnk_03005408) {
        if (!(gUnk_03005260 & 2))
            goto update;
        if (!(gUnk_03005260 & 1))
            goto update;
        if ((12 & gUnk_03005260) != 12)
            goto update;
        sub_08224FA4();
        return 0;
    } else
        gUnk_03005408 = 0;
update:
    {
        u32 state = task->state;
        switch (state) {
        case 0: {
            u16 procedure;
            u32 *active;
            u32 next;
            gUnk_030054B0 = state;
            active = &gUnk_03003A08;
            next = 1;
            *active = next;
            gUnk_030053E8 = state;
            task->counter = state;
            sub_0822502C();
            sub_0821B078();
            procedure = gUnk_030039F0;
            if (!procedure)
                goto no_procedure;
            sub_0821AD08(procedure, 0);
            if ((u32)procedure << 16)
                goto ran_procedure;
no_procedure:
            sub_0821B004();
ran_procedure:
            ;
            *active = state;
            task->state = next;
            gUnk_030053FC = state;
            break;
        }
    case 1:
            if (task->counter <= 0) {
                if (gUnk_030053E8) {
                    if (gUnk_0300523C & 2)
                        break;
                    if (!sub_08224F2C()) {
                        gUnk_030053DC = 0;
                        gUnk_03005414 = 0;
                        gUnk_030053FC = state;
                        {
                            u32 mask = ~14;
                            gUnk_0300523C = mask & gUnk_0300523C;
                        }
                        sub_0821A0F8(1);
                        task->counter = 3;
                        {
                            u16 *ptr = &gUnk_03004BD8;
                            u32 mask = ~0xE00;
                            *ptr = mask & *ptr;
                        }
                        gUnk_030051D8 = 0x40;
                        break;
                    }
                }
                if (!(gUnk_0300523C & 2))
                    gUnk_0300540C++;
                break;
            }
            task->counter--;
            if (task->counter > 0)
                break;
            sub_082250D8();
            sub_082250DC();
            if (gUnk_030053F4 & 0x200) {
                gUnk_030053F4 &= ~0x200;
                if (gUnk_0300523C & 8)
                    gUnk_0300523C &= ~8;
            }
            if (gUnk_030053E8 & 0x10)
                sub_0821B148();
            else if (gUnk_030053E8 & 0x100) {
                sub_0821B180();
                sub_0821B03C(*(s16 *)(gUnk_02000710 + 0x5A4));
            }
            task->state = 0;
            break;
        }
    }
    if (gUnk_03005404) {
        sub_08228928();
        gUnk_03005404 = 0;
    }
    if (*gUnk_030053F8 <= 0xFFFFFFFE)
        (*gUnk_030053F8)++;
    gUnk_030053E4++;
    if (gUnk_030053D8) {
        s32 *counter = (s32 *)(gUnk_02000710 + 0x614);
        if (*counter <= 0x7FFFFFFE)
            (*counter)++;
    }
    return 0;
}
