#include "global.h"
#include "player/player.h"
#define gPlayerActionCueTable gUnk_08E5FA6C
#define gPlayerActionCueTable gUnk_08E5FA6C
/* 64-byte records: a cue type every 12 bytes, starting at byte 4 (tentative) */
extern const u8 gUnk_08E5FA6C[][64];
void ActionRecord_CopyFromEntry(void *, void *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SelectActionCue(struct Player *p)
{
    struct PlayerActionDescriptor descriptor;
    const u8 *record;
    s32 offset;
    const u8 *type;
    s32 cue, result;

    ActionRecord_CopyFromEntry(p->statModifierContext.actionEntry, &descriptor);
    record = gUnk_08E5FA6C[descriptor.comboRecord];
    offset = p->actionResetCounter * 12 + 4;
    type = &record[offset];
    cue = -1;
    switch (p->tetherPhase) {
        case 0:
            if (*type == 1)
                cue = 0x15;
            else if (*type == 2)
                cue = 0x18;
            else if (*type == 3)
                cue = 0x1D;
            break;
        case 1:
        case 2:
        case 4:
        case 5:
            if (*type == 1) {
                if (p->tetherPhase == 4)
                    cue = 0x17;
                else
                    cue = 0x16;
            } else if (*type == 2)
                cue = 0x18;
            else if (*type == 3)
                cue = 0x1E;
            break;
        case 3:
            if (*type == 1)
                cue = 0x17;
            else if (*type == 2)
                cue = 0x1C;
            else if (*type == 3)
                cue = 0x1F;
            break;
    }
    if (cue != -1)
        result = ActorVariant_GetTableValue(p->actionMode, cue);
    else
        result = cue;
    return result;
}
