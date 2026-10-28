#include "global.h"
#include "ram.h"

/* tentative: reads the action mode from the script and installs its callback and input slot */

struct GameStateFC38 {
    u8 unk0[0x10];
    u16 menuOption; /* 0x10 */
    u8 unk12[0x5A8 - 0x12];
    u16 actionMode; /* 0x5A8 */
};

struct InputKeysFC38 {
    u16 keys[4];
};

struct PlayerFC38 {
    u8 unk0[0x18];
    s32 slot;
    u32 kind;
    u8 unk20[0x348 - 0x20];
    const u16 *inputKeys;
    u8 unk34C[0x418 - 0x34C];
    u8 actionMode;
    u8 unk419[0x433 - 0x419];
    u8 unk433;
    u8 unk434[0x45C - 0x434];
    u8 unk45C;
    u8 statGaugeFull;
    u8 unk45E;
    u8 historyDirectionMode;
    u8 actionRequestRestricted;
    u8 unk461;
    u8 unk462[2];
    u8 actionLatch464;
    u8 unk465[0x472 - 0x465];
    u8 timedEventSlot;
    u8 unk473[0x530 - 0x473];
    u16 scriptStatus[2];
    u16 unk534;
    u16 unk536;
    u16 unk538;
    u8 unk53A[2];
    u8 unk53C;
    u8 unk53D[0x54A - 0x53D];
    u8 catalogSavedOption;
    u8 unk54B[0xCA4 - 0x54B];
    void *updateCallback;
    u32 unkCA8;
};

extern struct GameStateFC38 *gGameState;
extern u32 gEngineFlags;
extern struct InputKeysFC38 gUnk_03005260[];
extern u16 gUnk_02000548[2];
extern u16 gUnk_02000534;
extern void *const gUnk_08610964[];

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Player_RecalculateDerivedStats(struct PlayerFC38 *);
void Player_UpdateLinkModeMovement(void);
void ActorSequence_Begin(struct PlayerFC38 *, s32, s32);
void Player_ClearDirectionHistory(struct PlayerFC38 *);

s32 Player_InitActionMode(struct PlayerFC38 *p)
{
    s32 i;

    p->kind = 1;
    p->statGaugeFull = 0;
    if (Script_SeekToKeyword(0x6B)) {
        p->actionMode = Script_GetValue();
        if ((u8)(p->actionMode - 1) <= 1)
            p->statGaugeFull = 1;
    } else {
        p->actionMode = gGameState->actionMode;
    }
    if (p->actionMode == 6)
        p->timedEventSlot = 1;
    else
        p->timedEventSlot = 0;
    Player_RecalculateDerivedStats(p);
    if (gEngineFlags & 0x1000) {
        void *cb = Player_UpdateLinkModeMovement;
        u16 *q, *base = gUnk_02000548;
        u16 zero = 0;
        q = base + 1;
        do {
            *q-- = zero;
        } while ((s32)q >= (s32)base);
        p->updateCallback = cb;
    } else {
        p->updateCallback = gUnk_08610964[p->actionMode];
    }
    p->unk45C = 1;
    ActorSequence_Begin(p, 0, 0);
    p->unkCA8 = 0;
    p->inputKeys = gUnk_03005260[p->slot].keys;
    Player_ClearDirectionHistory(p);
    if (p->slot == 0) {
        p->unk534 = gPlayerInteractionMode;
        p->unk536 = gPlayerScriptState;
        p->unk538 = gUnk_02000534;
        for (i = 0; i < 2; i++)
            p->scriptStatus[i] = gUnk_02000548[i];
    } else {
        s32 j;
        p->unk534 = 0;
        p->unk536 = 0;
        p->unk538 = 0;
        for (j = 0; j < 2; j++)
            p->scriptStatus[j] = 0;
    }
    p->catalogSavedOption = gGameState->menuOption;
    if (p->scriptStatus[1] != 0)
        p->catalogSavedOption = gPlayerSavedMenuOption;
    p->actionLatch464 = 0;
    p->unk53C = 0;
    p->unk433 = 0;
    p->historyDirectionMode = 0;
    p->actionRequestRestricted = 0;
    p->unk461 = 0;
    return 0;
}
