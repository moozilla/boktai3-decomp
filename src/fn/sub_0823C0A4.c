#include "global.h"

/* tentative: link-player variant of entering kind four: resets to idle and updates link counters */
#define LinkPlayer_ResetToIdleState sub_0823C0A4

struct LP {
    u8 unk0[0x18];
    s32 slot; /* 0x18 */
    u32 kind; /* 0x1C */
    u8 unk20[0x30];
    u32 unk50;   /* 0x50 */
    void *unk54; /* 0x54 */
    u8 unk58[0x274 - 0x58];
    u16 positionResetValue; /* 0x274 */
    u8 unk276[0x33E - 0x276];
    u8 actionSelectionStamp; /* 0x33E */
    u8 unk33F[5];
    u16 *actionConfig; /* 0x344 */
    u8 unk348[0x3A2 - 0x348];
    u8 heading, side, facing; /* 0x3A2 */
    u8 unk3A5[0x418 - 0x3A5];
    u8 actionMode; /* 0x418 */
    u8 unk419;
    u8 unk41A[0x458 - 0x41A];
    u8 tetherPhase; /* 0x458 */
    u8 unk459;
    u16 tetherFrame; /* 0x45A */
    u8 unk45C[0x48C - 0x45C];
    u8 movementVelocityLocked; /* 0x48C */
    u8 unk48D[0x4B6 - 0x48D];
    s16 cameraOffsetActive; /* 0x4B6 */
    u8 unk4B8[0x4E1 - 0x4B8];
    u8 unk4E1;
    u8 unk4E2[0x530 - 0x4E2];
    u16 scriptStatus[2]; /* 0x530 */
    u16 sceneMode;       /* 0x534 */
    u16 unk536;
    u16 active; /* 0x538 */
    u8 unk53A[0x549 - 0x53A];
    u8 unk549;
    u8 catalogSavedOption; /* 0x54A */
    u8 unk54B[3];
    u16 unk54E;
    u32 unk550;
    u8 unk554[0x5C0 - 0x554];
    void *unk5C0;
    u8 unk5C4[0xAC4 - 0x5C4];
    u16 unkAC4;
    u8 unkAC6[0xCA4 - 0xAC6];
    void *updateCallback; /* 0xCA4 */
};
void sub_0824261C(struct LP *);
void LinkPlayer_FlushPendingEffects(struct LP *);
void Player_StopLocalActionSounds(struct LP *);
void Player_ClearMovementLock(struct LP *);
void sub_081093FC(void *, s32);
s32 LinkSession_IncrementAlternateCounter(s32);
s32 LinkSession_IncrementCounter(s32);
void Player_NotifyFirstSelectedSlot(s32, u32);
void LinkPlayer_ResetToIdleState(struct LP *p)
{
    s32 i;
    u16 zero;
    u8 *q;
    if (p->actionMode == 4) {
        u32 z = 0;
        p->actionMode = 1;
        p->updateCallback = sub_0824261C;
        p->unk419 = z;
        p->unk54 = (u8 *)p + 0x8c;
        p->unk50 = z;
        p->unk5C0 = (u8 *)p + 0xac;
        p->unkAC4 = 0xFFFF;
    }
    p->facing |= 1;
    if (p->facing > 4) {
        p->heading = (8 - p->facing) >> 1;
        p->side = 1;
    } else {
        p->heading = p->facing >> 1;
        p->side = 0;
    }
    p->kind = 8;
    p->tetherPhase = 0;
    p->tetherFrame = 0;
    p->positionResetValue = 0;
    p->actionSelectionStamp = 0;
    LinkPlayer_FlushPendingEffects(p);
    Player_StopLocalActionSounds(p);
    if (p->cameraOffsetActive)
        Player_ClearMovementLock(p);
    p->sceneMode = 0;
    p->unk536 = 0;
    p->active = 0;
    p->unk4E1 = 0;
    p->movementVelocityLocked = 0;
    zero = 0;
    for (i = 1; i >= 0; i--)
        p->scriptStatus[i] = zero;
    p->catalogSavedOption = p->actionConfig[0x14 / 2];
    if (p->actionMode != 6) {
        q = &p->unk549;
        if ((s8)*q >= 0)
            sub_081093FC((u8 *)p + 0x30, (s8)*q);
        *q = 0xff;
    }
    LinkSession_IncrementAlternateCounter(p->slot);
    if (p->unk54E & 0xf) {
        if (p->unk54E & 1)
            LinkSession_IncrementCounter(0);
        else if (p->unk54E & 2)
            LinkSession_IncrementCounter(1);
        else if (p->unk54E & 4)
            LinkSession_IncrementCounter(2);
        else if (p->unk54E & 8)
            LinkSession_IncrementCounter(3);
        Player_NotifyFirstSelectedSlot((s16)p->unk54E, p->unk550);
    }
}
