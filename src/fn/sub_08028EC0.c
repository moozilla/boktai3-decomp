#include "global.h"
struct Actor {
    u8 pad0[0x3b9];
    u8 command;
    u8 pad3ba[2];
    int argument;
    u8 pad3c0[0x61c];
    void *handle;
};
struct Actor *sub_0802140C(int);
int Script_GetKeywordValue(int, int);
void sub_08022AF4(void *);
void BackgroundFlash_BeginAlternating(void *);
void BackgroundFlash_BeginAppearance(void *);
extern u32 gUnk_0300523C;
void sub_08028EC0(void)
{
    struct Actor *p = sub_0802140C(0);
    if (p && p->handle) {
        int a = Script_GetKeywordValue(115, 0);
        int b = Script_GetKeywordValue(104, 0);
        if (!p->command) {
            if (a)
                p->command = 3;
            else if (b)
                p->command = 1;
            else
                p->command = 2;
            p->argument = 0;
        }
    }
}
