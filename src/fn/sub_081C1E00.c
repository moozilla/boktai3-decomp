#include "global.h"
struct T {
    u8 f[0x30];
    u16 a30;
    u16 a32;
    u16 a34;
};
extern u32 gUnk_02000260;
extern struct T *volatile gUnk_02000580;
void StageIconDisplay_Create(u32, u32, void (*)(void *), u32);
void PlayerCue_Stand(void *, u32);
void sub_081C1D5C(void *);
void sub_081C1E00(void)
{
    u32 v = gUnk_02000260;
    if (v != 0) {
        StageIconDisplay_Create(0, 0, sub_081C1D5C, v);
        PlayerCue_Stand(gUnk_02000580, 7);
        gUnk_02000580->a30 = 0x380;
        gUnk_02000580->a32 = 0x100;
        gUnk_02000580->a34 = 0x580;
    }
}
