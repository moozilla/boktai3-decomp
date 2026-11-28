#include "global.h"
#include "player/player.h"
#define gPlayerActionCueTable gUnk_08E5FA6C
/* 64-byte records: a cue type every 12 bytes, starting at byte 4 (tentative) */
extern const u8 gUnk_08E5FA6C[][64];
void ActionRecord_CopyFromEntry(void *, void *);
s32 Player_IsActionCueReady(struct Player *p)
{
    struct PlayerActionDescriptor descriptor;
    const u8 *record;
    s32 offset;
    u32 type;

    ActionRecord_CopyFromEntry(p->statModifierContext.actionEntry, &descriptor);
    record = gUnk_08E5FA6C[descriptor.comboRecord];
    offset = p->actionResetCounter * 12 + 4;
    type = record[offset];
    if (type == 1) {
        if (p->animation.frameIndex == 3 && p->animation.elapsed > 2)
            return 1;
    } else if (type == 2) {
        if (p->animation.frameIndex == 2 && p->animation.elapsed > 4)
            return 1;
    } else if (type == 3) {
        if (p->animation.frameIndex > 4 && p->animation.elapsed > 7)
            return 1;
    }
    return 0;
}
