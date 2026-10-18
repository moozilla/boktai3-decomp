#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
#define SceneHud_TickStatTransfer   sub_08171888
#define SceneHud_DrawComparisonBars sub_08171014
#define gStatTransferProfiles       gUnk_08E60F80
struct StatTransferProfile {
    u8 data[16];
};
extern const struct StatTransferProfile gUnk_08E60F80[];
void PauseMenu_SmoothPoint(void *, void *, s32);
void GameState_EquipConsumableSlot(u32);
void Player_RefreshEquipmentWithOptions(void *, const void *, s32);
void sub_081716B4(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_08171258(void *);
void sub_08170D78(void *);
s32 Player_GetCandidateAttribute(s32, s32);
s32 Player_GetCurrentAttribute(s32);
s32 Inventory_GetConsumableId(u32);
void sub_08171014(void *, s32, s32, s16 *);
void sub_08166ED4(void *, int, int, int, int);
void sub_08166F84(void *, u32, u32, u32, u32);
void sub_08171888(struct SceneHudState *p)
{
    s16 differences[3];
    s32 i;
    if (!p->actionTransferDelay)
        Sound_PlaySong(0x38A);
    if (++p->actionTransferDelay < 12) {
        PauseMenu_SmoothPoint(&p->motionWidgets[0].position, &p->actionTargets[0],
                     12 - p->actionTransferDelay);
    } else {
        s32 zero;
        p->motionWidgets[0].flags |= 1;
        Sound_PlaySong(0x10C);
        GameState_EquipConsumableSlot(p->actionSelection);
        Player_RefreshEquipmentWithOptions(p->actionOwner, &gUnk_08E60F80[p->comparisonProfiles[gUnk_02000710->party[0]]],
                     -1);
        Hud_SetInteractionCallback(p, sub_081716B4, 1);
        sub_08171258(p);
        sub_08170D78(p);
        for (i = 0; i < 3; i++) {
            s32 value = Player_GetCandidateAttribute(i, Inventory_GetConsumableId(p->actionCursor));
            differences[i] = value - Player_GetCurrentAttribute(i);
        }
        sub_08171014(p, 10, 6, differences);
        zero = 0;
        sub_08166ED4(p, 8, 3, 0, zero);
        sub_08166F84(p, 8, 4, 0, zero);
    }
}
