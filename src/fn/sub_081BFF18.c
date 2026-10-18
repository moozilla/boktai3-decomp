#include "global.h"
void sub_081BF57C(u8 *);
void sub_081BF5DC(u8 *);
void Storage_LoadDisplayResources(u8 *);
void sub_081BF67C(u8 *);
void sub_081BF9C4(u8 *);
void sub_081BF51C(u8 *, u8 *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void sub_081BFC14(u8 *, s32);
void Sound_PlaySong(s32);
void Storage_SetUpdateCallback(u8 *, void (*)(void));
void Storage_UpdateOpeningDelay(void);
/* Screen setup: five sub-inits, sub_081BF51C on +0x17C8, ScreenTransition_SendCommand(0, 5,
   4, 4, 4, 0xFFFF, 0), song 0x118 and update Storage_UpdateOpeningDelay. */
u32 Storage_Init(u8 *p)
{
    sub_081BF57C(p);
    sub_081BF5DC(p);
    Storage_LoadDisplayResources(p);
    sub_081BF67C(p);
    sub_081BF9C4(p);
    sub_081BF51C(p, p + 0x17C8);
    ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, 0);
    sub_081BFC14(p, 1);
    Sound_PlaySong(0x118);
    Storage_SetUpdateCallback(p, Storage_UpdateOpeningDelay);
    return 0;
}
