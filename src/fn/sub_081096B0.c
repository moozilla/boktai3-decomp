#include "global.h"

struct TimedMessage {
    u8 unknown0[0x20];
    void (*update)(struct TimedMessage *);
    const u8 *followupScript;
    const u8 *script;
    u8 unknown2C[4];
    s32 window;
    u8 state, entering;
    u16 timer;
    u32 kind;
};

s32 LinkConnectionMenu_GetErrorMessageIndex(struct TimedMessage *p)
{
    u32 v = p->kind;
    switch (v) {
        case 3:
            return 0;
        case 0x10:
            return 1;
        default:
            return 2;
    }
}
