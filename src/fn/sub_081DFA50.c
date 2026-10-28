#include "global.h"
#include "menu_parameters.h"
s32 Script_ParseStringRef(u32);
const u8 *Text_LookupString(s32);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawText(s32, s32, u8 *);
struct PwLabels {
    u8 pad[0x370];
    u8 b0[0x100], b1[0x100], b2[0x100], b3[0x100];
    s32 offs[24];
};
void Password_BuildLabels(struct PwLabels *p)
{
    u8 *dst = p->b0;
    s32 i;
    for (i = 0; i < 24; i++) {
        s32 off = p->offs[i];
        const u8 *src;
        switch (i) {
            case 0:
                dst = p->b0;
                break;
            case 6:
                dst = p->b1;
                break;
            case 12:
                dst = p->b2;
                break;
            case 18:
                dst = p->b3;
                break;
        }
        src = Text_LookupString(Script_ParseStringRef(gSelectionMenuParameters.pBlock) + off);
        while (*src) {
            *dst = *src;
            dst++;
            src++;
        }
        *dst = 0;
    }
    Menu_EraseRect(2, 3, 0x1a, 4);
    Menu_DrawText(2, 3, p->b0);
    Menu_DrawText(0x10, 3, p->b1);
    Menu_DrawText(2, 5, p->b2);
    Menu_DrawText(0x10, 5, p->b3);
}
