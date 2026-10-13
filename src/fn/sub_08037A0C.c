#include "global.h"
struct P {
    u8 pad[0x1a];
    u8 b1a;
    u8 pad1[5];
    u32 w20;
    u8 pad2[0x4cc];
    u32 w4f0;
};
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void Sound_PlaySong(u32);
void sub_08036C38(void);
void Script_Call(u32, u32);
void sub_08037A0C(struct P *p)
{
    if (p->b1a != 0) {
        p->b1a = 0;
        Sound_PlaySong(0xde);
        sub_08036C38();
    }
    if (p->w20 == 4) {
        sub_082279A8(3, 4, 4, 4, p->w20, 0xFFFF, 0);
    }
    if (p->w20 == 0x18) {
        if (p->w4f0 != 0)
            Script_Call(p->w4f0, 0);
    }
    p->w20++;
}
