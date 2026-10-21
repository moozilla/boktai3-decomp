// MATCHING_CONCESSION: register-pinning
#include "global.h"

// foundry draft (worker best 96.2%); what differs is mostly register allocation and instruction order
#define gRandomIndex gUnk_03005308
#define gRandomTable gUnk_0203B400
struct SelectionPanel {
    u8 unknown0[0xD24];
    s8 column;
};
struct SelectionContext {
    u8 unknown0[0x53];
    u8 mode;
    u8 unknown54[0x1B8];
    u8 selection;
};
extern struct SelectionContext *gUnk_020004B4;
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
s32 Mod(s32, s32);
s32 SelectionPanel_GetSelection(struct SelectionPanel *);
void SelectionPanel_CommitSelection(struct SelectionPanel *p)
{
    struct SelectionContext *context = gUnk_020004B4;
    if (context && context->mode == 1) {
        u8 *selection = &context->selection;
        if (selection) {
            register u32 off asm("r1") = 0xD24;
            s8 *column;
            asm("" : "+r"(off));
            column = (s8 *)p + off;
            if (*column > 4) {
                u32 offset;
                gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
                offset = gUnk_03005308 * 2;
                *column = Mod(*(const u16 *)(offset + (u32)gUnk_0203B400), 5);
            }
            *selection = SelectionPanel_GetSelection(p);
        }
    }
}
