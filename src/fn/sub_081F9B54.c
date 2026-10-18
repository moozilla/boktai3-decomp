#include "global.h"
extern u32 gUnk_02000560;
extern const u32 gUnk_08613F48[];
s32 sub_081F79F8(void *);
void Callback_InvokeActor(void *, u32);
void Ratatosk_UpdateCombatMode(u8 *p)
{
    u32 m = 4;
    if (!(gUnk_02000560 & m)) {
        if (sub_081F79F8(p) == 0) {
            Callback_InvokeActor(p, gUnk_08613F48[p[0xad]]);
        }
    }
}
