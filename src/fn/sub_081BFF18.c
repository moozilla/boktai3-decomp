#include "global.h"
void sub_081BF57C(u8 *);
void sub_081BF5DC(u8 *);
void sub_081BF634(u8 *);
void sub_081BF67C(u8 *);
void sub_081BF9C4(u8 *);
void sub_081BF51C(u8 *, u8 *);
void sub_082279A8(s32, s32, s32, s32, s32, s32, s32);
void sub_081BFC14(u8 *, s32);
void Sound_PlaySong(s32);
void sub_081BDB10(u8 *, void (*)(void));
void sub_081BFCD4(void);
/* Screen setup: five sub-inits, sub_081BF51C on +0x17C8, sub_082279A8(0, 5,
   4, 4, 4, 0xFFFF, 0), song 0x118 and update sub_081BFCD4. */
u32 sub_081BFF18(u8 *p)
{
    sub_081BF57C(p);
    sub_081BF5DC(p);
    sub_081BF634(p);
    sub_081BF67C(p);
    sub_081BF9C4(p);
    sub_081BF51C(p, p + 0x17C8);
    sub_082279A8(0, 5, 4, 4, 4, 0xFFFF, 0);
    sub_081BFC14(p, 1);
    Sound_PlaySong(0x118);
    sub_081BDB10(p, sub_081BFCD4);
    return 0;
}
