#define SelectionPanel_GetSelection sub_0810E1A0
#include "global.h"
struct SelectionPanel {
    u8 unknown0[0xD24];
    s8 column;
    u8 unknownD25;
    s8 page;
};
s32 sub_0810E1A0(struct SelectionPanel *p)
{
    s32 index;
    switch (p->page) {
    default:
        return 0;
    case 0:
        index = 0;
        break;
    case 1:
        index = 6;
        break;
    }
    index += p->column;
    return index;
}
