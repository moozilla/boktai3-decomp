#include "global.h"
#include "game_state.h"
#include "input.h"
#include "engine_globals.h"

extern const u16 gUnk_0203B400[];
extern const u8 gUnk_0824F960[];
extern const u8 gUnk_0824F968[];
extern const u8 gUnk_0824F970[];
extern const u8 gUnk_0824F978[];
extern const u8 gUnk_0824F984[];
extern const u8 gUnk_0824F990[];

struct Sub081C0CE4Entry {
    u8 unk0[8];
    u32 flags8;
    u8 unkC[0x54];
};

struct Sub081C0CE4Task {
    u8 unk00[0x18];
    u8 keyframeData[0x40];
    void *layerMap;
    u8 unk5C[4];
    u32 flags;
    void *script;
    u8 unk68[8];
    s32 state;
    s32 timer;
    u32 unk78;
    u8 digit7C;
    u8 digit7D;
    u8 unk7E;
    u8 digit7F;
    u8 digit80;
    u8 unk81;
    u8 digit82;
    u8 digit83;
    u8 unk84;
    u8 label85[11];
    struct Sub081C0CE4Entry entries[10];
    u32 soundCued;
};

static inline void sub081C0CE4_Enter(struct Sub081C0CE4Task *task, s32 state)
{
    task->state = state;
    task->timer = 0;
    task->flags |= 2;
}

static inline u8 sub081C0CE4_RandomDigit(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return Mod(gUnk_0203B400[gRandomIndex], 10) + '0';
}

#ifdef NONMATCHING
// fuzzy: 63.7%
// scope shape 160 (differences once low registers are set aside)
// confidence=low: stats-screen task step: draws the play-time label, animates the entry list, and steps the screen state machine
void sub_081C0CE4(struct Sub081C0CE4Task *task)
{
    u8 text[12];
    u8 blank[12];
    s32 wasSet;
    s32 wasBit;
    s32 i;
    s32 state;
    const u8 *src;
    s16 v;

    wasSet = task->flags & 1;
    task->flags &= ~1;
    if (wasSet) {
        Bg_ClearTilemap(0);
        BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
        Bg_ClearTilemap(2);
        BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
        Bg_ClearTilemap(3);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        Bg_SetLayerMap(2, task->layerMap, 0xb);
        BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
        task->unk78 = 0xb;
        GameState_UpdateCounterBalanceRank();
        GameStats_SelectLeadingCounter();
        PlayStats_DrawLabels(task);
        task->state = 0;
        task->timer = 0;
        task->flags |= 2;
    }

    state = task->state;
    switch (state) {
        case 0:
            wasBit = task->flags & 2;
            task->flags &= ~2;
            if (wasBit)
                ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, state);
            if (task->timer > 0x20)
                sub081C0CE4_Enter(task, 1);
            break;
        case 1:
            wasBit = task->flags & 2;
            task->flags &= ~2;
            if (wasBit)
                task->soundCued = 0;
            text[2] = ':';
            text[5] = ':';
            if (task->timer <= 0x3b) {
                text[0] = sub081C0CE4_RandomDigit();
                text[1] = sub081C0CE4_RandomDigit();
            } else {
                text[0] = task->digit7C;
                text[1] = task->digit7D;
            }
            if (task->timer <= 0x68) {
                text[3] = sub081C0CE4_RandomDigit();
                text[4] = sub081C0CE4_RandomDigit();
            } else {
                text[3] = task->digit7F;
                text[4] = task->digit80;
            }
            if (task->timer <= 0x95) {
                text[6] = sub081C0CE4_RandomDigit();
                text[7] = sub081C0CE4_RandomDigit();
                if (task->timer & 1)
                    Sound_PlaySong(0x273);
            } else {
                text[6] = task->digit82;
                text[7] = task->digit83;
                if (!task->soundCued) {
                    Sound_PlaySong(0x167);
                    task->soundCued = 1;
                }
            }
            text[8] = 0;
            Menu_EraseRect(0x14, 4, 8, 2);
            Menu_DrawText(0x14, 4, text);
            if (task->timer > 0xd2 || (gPadState[1] & 1)) {
                if (!task->soundCued) {
                    Sound_PlaySong(0x167);
                    task->soundCued = 1;
                }
                sub081C0CE4_Enter(task, 2);
            }
            break;
        case 2:
            wasBit = task->flags & 2;
            task->flags &= ~2;
            if (wasBit) {
                Menu_EraseRect(0x14, 4, 8, 2);
                Menu_DrawText(0x14, 4, &task->digit7C);
                task->soundCued = 0;
                blank[0] = ' ';
                blank[1] = ' ';
                blank[2] = ',';
                blank[3] = ' ';
                blank[4] = ' ';
                blank[5] = ' ';
                blank[6] = ',';
                blank[7] = ' ';
                blank[8] = ' ';
                blank[9] = ' ';
                blank[10] = 0;
                Menu_EraseRect(0x12, 7, 0xa, 2);
                Menu_DrawText(0x12, 7, blank);
            }
            if (task->timer <= 0x3b) {
                for (i = 0; i < 8; i++) {
                    gRandomIndex = (gRandomIndex + 1) & 1023;
                    Anim_LoadKeyframe(&task->entries[i], task->keyframeData,
                                      (u16)Mod(gUnk_0203B400[gRandomIndex], 10), 1);
                    task->entries[i].flags8 &= ~1;
                }
                if (task->timer & 1)
                    Sound_PlaySong(0x273);
            } else {
                PlayStats_HideEntryAnimations(task);
                Menu_EraseRect(0x12, 7, 0xa, 2);
                Menu_DrawText(0x12, 7, task->label85);
                if (!task->soundCued) {
                    Sound_PlaySong(0x167);
                    task->soundCued = 1;
                }
                sub081C0CE4_Enter(task, 3);
            }
            if (gPadState[1] & 1) {
                if (!task->soundCued) {
                    Sound_PlaySong(0x167);
                    task->soundCued = 1;
                }
                sub081C0CE4_Enter(task, 3);
            }
            break;
        case 3:
            wasBit = task->flags & 2;
            task->flags &= ~2;
            if (wasBit) {
                PlayStats_HideEntryAnimations(task);
                Menu_EraseRect(0x12, 7, 0xa, 2);
                Menu_DrawText(0x12, 7, task->label85);
            }
            if (task->timer > 0x3c || (gPadState[1] & 1)) {
                Menu_EraseRect(0x17, 0xa, 8, 2);
                v = *(s16 *)((u8 *)gGameState + 0x660);
                if (v == 1 || v == 0)
                    Menu_DrawText(0x17, 0xa, v == 1 ? gUnk_0824F968 : gUnk_0824F960);
                else
                    Menu_DrawText(0x17, 0xa, gUnk_0824F970);
                Sound_PlaySong(0x167);
                sub081C0CE4_Enter(task, 4);
            }
            break;
        case 4:
            task->flags &= ~2;
            if (task->timer > 0x3c || (gPadState[1] & 1)) {
                Menu_EraseRect(0x13, 0xd, 0xc, 2);
                v = *(s16 *)((u8 *)gGameState + 0x662);
                if (v == 4 || v == 0)
                    Menu_DrawText(0x14, 0xd, v == 4 ? gUnk_0824F984 : gUnk_0824F978);
                else
                    Menu_DrawText(0x14, 0xd, gUnk_0824F990);
                Sound_PlaySong(0x167);
                sub081C0CE4_Enter(task, 5);
            }
            break;
        case 5:
            task->flags &= ~2;
            if (task->timer > 0x3c) {
                task->entries[9].flags8 &= ~1;
                sub081C0CE4_Enter(task, 6);
            }
            break;
        case 6:
            task->flags &= ~2;
            if (gPadState[1] & 8) {
                Sound_PlaySong(0xdd);
                sub081C0CE4_Enter(task, 7);
            }
            break;
        case 7:
            wasBit = task->flags & 2;
            task->flags &= ~2;
            if (wasBit)
                ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xFFFF, 0);
            if (task->timer > 0x20)
                sub081C0CE4_Enter(task, 0x63);
            break;
        case 0x63:
            task->flags &= ~2;
            if (task->timer > 0x1e) {
                Script_Call(task->script, 0);
                Bg_ClearTilemap(0);
                BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
                Bg_ClearTilemap(2);
                BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
                Bg_ClearTilemap(3);
                BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
                Task_Kill(task);
            }
            break;
        default:
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081C0CE4);
#endif
