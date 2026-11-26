#ifndef GUARD_WIRELESS_GAME_ITEMS_AND_DISPLAY_H
#define GUARD_WIRELESS_GAME_ITEMS_AND_DISPLAY_H
#include "engine/shared/engine.h"
#include "engine/shared/scene_input_widget.h"
#include "link/wireless_trade_and_selection_menu/wireless_trade_menu.h"

/* Layouts recovered from the item state handlers and display lifecycle.
 * Interpretations are tentative; unknown ranges retain their offsets. */
struct WirelessGamePair {
    u32 x, y;
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
enum WirelessGameItemState {
    WIRELESS_GAME_ITEM_HIDDEN = 0,
    WIRELESS_GAME_ITEM_STEADY = 1,
    WIRELESS_GAME_ITEM_BLINKING = 2,
    WIRELESS_GAME_ITEM_MERGING = 3,
    WIRELESS_GAME_ITEM_OUTGOING = 4,
    WIRELESS_GAME_ITEM_INCOMING = 5,
    WIRELESS_GAME_ITEM_SCATTERED = 6,
    WIRELESS_GAME_ITEM_TEMPORARY = 7,
};

struct WirelessGameItem {
    u8 state, entered, pendingFlag, count, shakeTicks, unk5;
    s16 arcVelocity;
    u16 timer;
    u8 unkA[2];
    struct ContactPosition start;
    struct ContactPosition end;
    struct ContactPosition drift;
    struct WirelessGameSprite sprite;
    struct WirelessGameDrawing drawing;
};
static inline void WirelessGameItem_EnterState(struct WirelessGameItem *item, u8 state)
{
    item->state = state;
    item->entered = 1;
    item->timer = 0;
}

struct WirelessGameItems {
    struct Task task;
    u8 current[4], pending[4];
    u8 sprite[0x1c]; /* animation-data descriptor, not a sprite instance */
    struct SceneInputResource resource;
    struct WirelessGameItem items[16];
};
struct WirelessGameDisplay {
    struct Task task;
    u32 timer;
    u8 state, entered, finished, unk1F;
    struct SceneInputResource resource;
    struct WirelessGameDrawing drawings[4];
    struct SceneInputResource extraResource;
    struct WirelessGameDrawing extra[2];
};
/* 0x02000100 */
extern struct WirelessGameItems *gWirelessGameItems;
int WirelessGameItems_QueueLocalItem(struct ContactPosition *, int, int);
int WirelessGameDisplay_Init(struct WirelessGameDisplay *);

void WirelessGame_AddItem(void *p, int index);
int WirelessGame_TryAddCyclingItem(void *owner);
void WirelessGame_NotifyPlayerDefeated(void);
void WirelessGame_NotifyOpponentDefeated(void);
u32 WirelessGame_ConsumePendingEvent(void);
u32 WirelessGame_GetRemotePlayerHealthPercent(void);
u32 WirelessGame_GetRemoteOpponentHealthPercent(void);
u32 WirelessGame_IsItemToggleEnabled(void);
int WirelessGame_ToggleItemMode(void);
u32 WirelessGame_GetRoundCount(void);
void WirelessGame_SuppressPlayerReset(void);
void WirelessGame_AddWeightedScore(int index, int amount);
u32 WirelessGame_IsRoundActive(void);
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
