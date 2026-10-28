#include "global.h"
#include "engine.h"
#include "area_effect_link.h"
#include "ram.h"

// foundry draft (worker best 96.2%); what differs is mostly register allocation and instruction order
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
s32 Mod(s32, s32);
s32 SelectionPanel_GetSelection(struct SelectionPanel *);
void SelectionPanel_CommitSelection(struct SelectionPanel *p)
{
    unsigned int two;
    struct SelectionContext *context = gLinkSession;
    if (context && context->mode == 1) {
        if (&context->selection) {
            u32 off = 0xD24;
            s8 *column;
            column = (s8 *)p + off;
            if (*(*(&column)) > 4) {
                u32 offset;
                gRandomIndex = (gRandomIndex + 1) & 1023;
                two = 2;
                offset = gRandomIndex * two;
                *column = Mod(*(const u16 *)(offset + (u32)((const u16 *)0x0203B400)), 5);
            }
            *(&context->selection) = SelectionPanel_GetSelection(p);
        }
    }
}
