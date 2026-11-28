#include "global.h"
#include "player/player.h"
/* 64-byte records: a cue type every 12 bytes, starting at byte 4 (tentative) */
extern const u8 gUnk_08E5FA6C[][64];
void ActionRecord_CopyFromEntry(void *, void *);

s32 Player_HasWeaponComboEntry(struct Player *p)
{
    struct PlayerActionDescriptor descriptor;
    const u8 *record;
    s32 offset;

    if (p->actionResetCounter <= 3) {
        ActionRecord_CopyFromEntry(p->statModifierContext.actionEntry, &descriptor);
        record = gUnk_08E5FA6C[descriptor.comboRecord];
        offset = p->actionResetCounter * 12 + 0x10; /* the next step's cue type */
        if (record[offset] != 0)
            return TRUE;
    }
    return FALSE;
}
