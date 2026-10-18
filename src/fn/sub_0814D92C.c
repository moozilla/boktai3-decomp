#include "global.h"
#include "player.h"
#define Player_IsActionCueReady sub_0814D92C
#define gPlayerActionCueTable   gUnk_08E5FA6C
struct PlayerActionCueInfo {
    u8 unknown0[0x15];
    u8 cueIndex;
    u8 unknown16[0xA];
};
extern const u8 gUnk_08E5FA6C[];
void sub_0822D764(void *, void *);
s32 sub_0814D92C(struct Player *p)
{
    struct PlayerActionCueInfo info;
    u32 entry, offset;
    const u8 *record;
    sub_0822D764(p->statModifierContext.actionEntry, &info);
    record = gUnk_08E5FA6C + info.cueIndex * 64;
    offset = p->actionResetCounter * 12 + 4;
    record = (const u8 *)((u32)record + offset);
    entry = *record;
    if (entry == 1) {
        if (p->actionCueFlags == 3 && p->actionCueCounter > 2)
            return 1;
    } else if (entry == 2) {
        if (p->actionCueFlags == 2 && p->actionCueCounter > 4)
            return 1;
    } else if (entry == 3) {
        if (p->actionCueFlags > 4 && p->actionCueCounter > 7)
            return 1;
    }
    return 0;
}
