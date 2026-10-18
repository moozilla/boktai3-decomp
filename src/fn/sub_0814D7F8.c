#include "global.h"
#include "player.h"
#define Player_SelectActionCue sub_0814D7F8
#define gPlayerActionCueTable  gUnk_08E5FA6C
struct PlayerActionCueInfo {
    u8 unknown0[0x15];
    u8 cueIndex;
    u8 unknown16[0xA];
};
extern const u8 gUnk_08E5FA6C[];
void sub_0822D764(void *, void *);
s32 sub_081419D4(u32, s32);
s32 sub_0814D7F8(struct Player *p)
{
    struct PlayerActionCueInfo info;
    u32 entry;
    s32 cue, result;
    u32 offset;
    sub_0822D764(p->statModifierContext.actionEntry, &info);
    entry = info.cueIndex;
    entry <<= 6;
    entry += (u32)gUnk_08E5FA6C;
    offset = p->actionResetCounter * 12 + 4;
    entry += offset;
    cue = -1;
    switch (p->tetherPhase) {
        case 0:
            entry = *(const u8 *)entry;
            if (entry == 1)
                cue = 0x15;
            else if (entry == 2)
                cue = 0x18;
            else if (entry == 3)
                cue = 0x1D;
            break;
        case 1:
        case 2:
        case 4:
        case 5:
            entry = *(const u8 *)entry;
            if (entry == 1) {
                u32 phase = p->tetherPhase;
                cue = 0x16;
                if (phase == 4)
                    cue = 0x17;
            } else if (entry == 2)
                cue = 0x18;
            else if (entry == 3)
                cue = 0x1E;
            break;
        case 3:
            entry = *(const u8 *)entry;
            if (entry == 1)
                cue = 0x17;
            else if (entry == 2)
                cue = 0x1C;
            else if (entry == 3)
                cue = 0x1F;
            break;
    }
    if (cue != -1)
        result = sub_081419D4(p->actionMode, cue);
    else
        result = cue;
    return result;
}
