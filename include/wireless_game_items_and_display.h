#ifndef GUARD_WIRELESS_GAME_ITEMS_AND_DISPLAY_H
#define GUARD_WIRELESS_GAME_ITEMS_AND_DISPLAY_H
#include "engine.h"

/* Layouts recovered from the item state handlers and display lifecycle.
 * Interpretations are tentative; unknown ranges retain their offsets. */
struct WirelessGameResource {
    u32 words[8];
};
struct WirelessGamePair {
    u32 x, y;
};
struct WirelessGameVector {
    s16 x, y, z;
};
struct WirelessGamePosition {
    u16 x, y, z, unk6;
};
struct WirelessGameSprite {
    u32 flags;
    u8 unk4[3], visible;
    u8 unk8[8];
    u16 value;
    u8 unk12[10];
    u32 x, y;
    u8 unk24[8];
};
struct WirelessGameDrawing {
    u8 unk0[8];
    u32 flags;
    u8 unkC[0xe];
    u8 changed;
    u8 unk1B[5];
    union {
        struct WirelessGamePair words;
        struct WirelessGamePosition coords;
    } position;
    u8 unk28[0x38];
};
struct WirelessGameItem {
    u8 state, entered, pendingFlag, count, shakeTicks, unk5;
    s16 arcVelocity;
    u16 timer;
    u8 unkA[2];
    struct WirelessGameVector start;
    struct WirelessGameVector end;
    struct WirelessGameVector drift;
    struct WirelessGameSprite sprite;
    struct WirelessGameDrawing drawing;
};
struct WirelessGameItems {
    struct Task task;
    u8 current[4], pending[4];
    u8 sprite[0x1c]; /* animation-data descriptor, not a sprite instance */
    struct WirelessGameResource resource;
    struct WirelessGameItem items[16];
};
struct WirelessGameDisplay {
    struct Task task;
    u32 timer;
    u8 state, entered, finished, unk1F;
    struct WirelessGameResource resource;
    struct WirelessGameDrawing drawings[4];
    struct WirelessGameResource extraResource;
    struct WirelessGameDrawing extra[2];
};
struct WirelessGameRemoteStats {
    u8 unk0[4];
    u8 playerHealthPercent, opponentHealthPercent;
};
struct WirelessGameExchange {
    u8 unk0[0x1e];
    u16 roundCount;
    u8 unk20[4];
    u32 timer;
    u8 state, entered;
    u8 unk2A[3];
    u8 playerDefeated, opponentDefeated, unk2F;
    u8 pendingEvent, unk31, enabled;
    u8 unk33[9];
    u8 counts[4];
    u8 unk40[2];
    u8 itemMode;
    u8 unk43[5];
    struct WirelessGameRemoteStats remote;
    u8 unk50[0x10];
    struct WirelessGameDisplay *display;
    u8 unk64[0x78];
    s32 total;
    u8 unkE0[2];
    u16 weights[4];
    u8 choice, counter;
};
struct WirelessGameExchange *WirelessExchange_Get(void);
/* 0x02000100 */
extern struct WirelessGameItems *gWirelessGameItems;
int WirelessGameItems_QueueLocalItem(struct WirelessGameVector *, int, int);
int WirelessGameDisplay_Init(struct WirelessGameDisplay *);

void WirelessGame_AddItem(void *p, int index);
int WirelessGame_TryAddCyclingItem(void *owner);
void WirelessGame_NotifyPlayerDefeated(void);
void WirelessGame_NotifyOpponentDefeated(void);
u32 sub_08039708(void);
u32 WirelessGame_GetRemotePlayerHealthPercent(void);
u32 WirelessGame_GetRemoteOpponentHealthPercent(void);
u32 WirelessGame_IsItemToggleEnabled(void);
int WirelessGame_ToggleItemMode(void);
u32 WirelessGame_GetRoundCount(void);
void sub_080397C0(void);
void WirelessGame_AddWeightedScore(int index, int amount);
u32 sub_08039800(void);
void WirelessGame_ResetCyclingItemLatch(void);
void WirelessGame_InitDisplayAndLinkCallbacks(struct WirelessGameExchange *p);
void WirelessGame_RestartSession(void);
void WirelessGame_BeginResults(void);
u32 WirelessGame_GetSelectedEquipmentEffect(void);
void WirelessGameItem_SetRandomArcVelocity(struct WirelessGameItem *arg, int baseArg, int mask);
void WirelessGameItem_SetRandomNegativeArcVelocity(struct WirelessGameItem *p, int base, int mask);
void WirelessGameItems_ApplyMotionWave(struct WirelessGamePosition *arg, int angleArg, int scale);
void WirelessGameItems_UpdateHiddenItem(struct WirelessGameItems *p, struct WirelessGameItem *q);
void WirelessGameItems_UpdateBlinkingItem(struct WirelessGameItems *p, struct WirelessGameItem *q,
                                          int index);
void WirelessGameItems_UpdateItemMerge(struct WirelessGameItems *owner, struct WirelessGameItem *p,
                                       int indexArg);
void WirelessGameItems_UpdateOutgoingItem(struct WirelessGameItems *owner,
                                          struct WirelessGameItem *p, int index);
void WirelessGameItems_UpdateIncomingItem(struct WirelessGameItems *owner,
                                          struct WirelessGameItem *p, int indexArg);
void WirelessGameItems_UpdateTemporaryItem(struct WirelessGameItems *p, struct WirelessGameItem *q,
                                           int index);
void WirelessGameItems_InitItem(struct WirelessGameItems *owner, struct WirelessGameItem *p,
                                int index);
void WirelessGameItems_DestroyItem(struct WirelessGameItems *a, struct WirelessGameItem *p,
                                   int index);
void WirelessGameItems_PublishPendingEffects(struct WirelessGameItems *p);
int WirelessGameItems_Update(struct WirelessGameItems *p);
int WirelessGameItems_Destroy(struct WirelessGameItems *p);
int WirelessGameItems_Init(struct WirelessGameItems *p);
void *WirelessGameItems_Create(int unused0, int unused1);
int WirelessGameItems_QueueIncomingItem(int indexArg, int amountArg, int pendingArg);
void WirelessGameItems_StartOutgoingItems(void);
void WirelessGameDisplay_UpdateHidden(struct WirelessGameDisplay *p);
void WirelessGameDisplay_UpdateStaticNotice(struct WirelessGameDisplay *p);
void WirelessGameDisplay_UpdateAlternatingNotice(struct WirelessGameDisplay *p);
void WirelessGameDisplay_UpdateRoundAnnouncement(struct WirelessGameDisplay *p);
void WirelessGameDisplay_UpdateShakeAnimation(struct WirelessGameDisplay *arg);
void WirelessGameDisplay_UpdateChimeAnimation(struct WirelessGameDisplay *p);
int WirelessGameDisplay_Update(struct WirelessGameDisplay *p);
int WirelessGameDisplay_Destroy(struct WirelessGameDisplay *p);
void *WirelessGameDisplay_Create(void);
#endif
