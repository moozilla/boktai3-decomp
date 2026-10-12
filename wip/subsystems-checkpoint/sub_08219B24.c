#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define Input_PollButtons sub_08219B24
struct Buttons { u16 held, pressed, released, unused; };
extern u16 gUnk_03005270, gUnk_03005274[];
extern u32 gUnk_03005250, gUnk_0300523C;
extern struct Buttons gUnk_03005260[];
static inline u16 allKeys(void) { return 0x3FF; }
void Input_PollButtons(void)
{
    register s32 i asm("r4");
    register struct Buttons *buttons asm("r1");
    u16 held;
    gUnk_03005270 = *(volatile u16 *)0x04000130;
    if (!gUnk_03005250) {
        u32 previous;
        held = (u16)(0x3FF ^ gUnk_03005270);
        buttons = gUnk_03005260;
        previous = buttons->held;
        buttons->held = held;
        buttons->pressed = held & ~previous;
        buttons->released = previous & ~held;
        held = 0;
        buttons++;
        i = 0;
        do {
            previous = buttons->held;
            buttons->held = held;
            buttons->pressed = held;
            buttons->released = previous;
            buttons++;
            i--;
        } while (i >= 0);
    } else if (!(gUnk_0300523C & 1)) {
        u32 absent;
        u16 *input;
        u32 mask;
        
        i = 0;
        absent = 0xFFFF;
        input = gUnk_03005274;
        mask = allKeys();
        buttons = gUnk_03005260;
        do {
            u32 previous;
            u32 raw = *input;
            if (raw == absent) held = 0;
            else held = raw ^ mask;
            previous = buttons->held;
            buttons->held = held;
            buttons->pressed = held & ~previous;
            buttons->released = previous & ~held;
            input++;
            buttons++;
            i++;
        } while (i <= 1);
    }
}
