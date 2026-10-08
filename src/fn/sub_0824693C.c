#include "global.h"
u32 sub_082440D8(u32); void sub_08244150(void (*)(void)); void sub_08244510(void); void sub_08245630(u32, u32);
void sub_0824693C(void) { u16 r = sub_082440D8(1); if (r == 1) { sub_08244150((void (*)(void))sub_08245630); sub_08244510(); } else sub_08245630(0x27, 0); }
