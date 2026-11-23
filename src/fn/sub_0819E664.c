#include "global.h"
#include "player_slot_status.h"
extern const u32 gUnk_08611E04[];
s32 Hresvelgr_TryStartReaction(void *);
void Callback_InvokeActor(void *, u32);
void Hresvelgr_UpdateActiveMode(u8 *p)
{
    if (!PlayerSlot_ReadStatus(0, 4)) {
        if (Hresvelgr_TryStartReaction(p) == 0) {
            Callback_InvokeActor(p, gUnk_08611E04[p[0xad]]);
        }
    }
}
