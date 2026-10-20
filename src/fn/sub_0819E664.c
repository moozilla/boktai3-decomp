#include "global.h"
extern u32 gUnk_02000560;
extern const u32 gUnk_08611E04[];
s32 Hresvelgr_TryStartReaction(void *);
void Callback_InvokeActor(void *, u32);
void Hresvelgr_UpdateActiveMode(u8 *p)
{
    u32 m = 4;
    if (!(gUnk_02000560 & m)) {
        if (Hresvelgr_TryStartReaction(p) == 0) {
            Callback_InvokeActor(p, gUnk_08611E04[p[0xad]]);
        }
    }
}
