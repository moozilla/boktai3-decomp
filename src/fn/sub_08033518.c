#include "global.h"
#include "ram.h"
struct TextState {
    u8 pad0[0xd];
    u8 count;
    u8 padE[0xba];
    u32 values[8];
};
struct Window {
    u8 pad0[0x18];
    struct TextState text;
};
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(s32);
u32 Script_GetPc(void);

s32 sub_08033518(void)
{
    struct Window *p = gScrollingDialogue;
    if (p != 0) {
        struct TextState *t = &p->text;
        t->count = 0;
        if (Script_SeekToKeyword(0x70)) {
            while (Script_GetPc()) {
                t->values[t->count] = Script_GetValue();
                t->count++;
            }
            return 0;
        }
    }
    return -1;
}
