#include "global.h"
#include "game_state.h"
#include "input.h"
#include "engine_globals.h"
#include "ram.h"
#include "debug.h"
#ifdef NONMATCHING
// fuzzy: 98.7%

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

static inline s32 Task_TakeFlag(struct Sub081C0CE4Task *task, u32 bit)
{
    s32 set = (task->flags & bit) != 0;
    task->flags &= ~bit;
    return set;
}
static inline void Sub081C0CE4Entry_Show(struct Sub081C0CE4Entry *entry)
{
    entry->flags8 &= ~1;
}

static inline void sub081C0CE4_Enter(struct Sub081C0CE4Task *task, s32 state)
{
    task->state = state;
    task->timer = 0;
    {
        u32 bit = 2;
        task->flags |= bit;
    }
}

static inline s32 sub081C0CE4_RandomMod10(void)
{
    const u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return Mod(table[gRandomIndex], 10);
}

// scope: 9 ROM / 9 draft insns differ, shape 16 (case-2 loop invariants r8/sb, case-4 gGameState address hoisted)
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

    wasSet = Task_TakeFlag(task, 1);
    if (wasSet) {
        Bg_ClearTilemap(0);
        BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
        Bg_ClearTilemap(2);
        BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
        Bg_ClearTilemap(3);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        {
            u32 layer = 0xb;
            Bg_SetLayerMap(2, task->layerMap, layer);
            BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
            task->unk78 = layer;
        }
        GameState_UpdateCounterBalanceRank();
        GameStats_SelectLeadingCounter();
        PlayStats_DrawLabels(task);
        sub081C0CE4_Enter(task, 0);
    }

    state = task->state;
    switch (state) {
        case 0:
            wasBit = Task_TakeFlag(task, 2);
            if (wasBit)
                ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, state);
            if (task->timer > 0x20)
                sub081C0CE4_Enter(task, 1);
            break;
        case 1:
            wasBit = Task_TakeFlag(task, 2);
            if (wasBit)
                task->soundCued = 0;
            text[2] = text[5] = ':';
            if (task->timer <= 0x3b) {
                text[0] = sub081C0CE4_RandomMod10() + '0';
                text[1] = sub081C0CE4_RandomMod10() + '0';
            } else {
                text[0] = task->digit7C;
                text[1] = task->digit7D;
            }
            if (task->timer <= 0x68) {
                text[3] = sub081C0CE4_RandomMod10() + '0';
                text[4] = sub081C0CE4_RandomMod10() + '0';
            } else {
                text[3] = task->digit7F;
                text[4] = task->digit80;
            }
            if (task->timer <= 0x95) {
                text[6] = sub081C0CE4_RandomMod10() + '0';
                text[7] = sub081C0CE4_RandomMod10() + '0';
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
            wasBit = Task_TakeFlag(task, 2);
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
                {
                    struct Sub081C0CE4Entry *entry = task->entries;
                    for (i = 0; i < 8; i++, entry++) {
                        Anim_LoadKeyframe(entry, task->keyframeData, (u16)sub081C0CE4_RandomMod10(),
                                          1);
                        Sub081C0CE4Entry_Show(entry);
                    }
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
            wasBit = Task_TakeFlag(task, 2);
            if (wasBit) {
                PlayStats_HideEntryAnimations(task);
                Menu_EraseRect(0x12, 7, 0xa, 2);
                Menu_DrawText(0x12, 7, task->label85);
            }
            if (task->timer > 0x3c || (gPadState[1] & 1)) {
                Menu_EraseRect(0x17, 0xa, 8, 2);
                switch (*(s16 *)((u8 *)gGameState + 0x660)) {
                    case 0:
                        Menu_DrawText(0x17, 0xa, gUnk_0824F960);
                        break;
                    case 1:
                        Menu_DrawText(0x17, 0xa, gUnk_0824F968);
                        break;
                    case 2:
                    default:
                        Menu_DrawText(0x17, 0xa, gUnk_0824F970);
                        break;
                }
                Sound_PlaySong(0x167);
                sub081C0CE4_Enter(task, 4);
            }
            break;
        case 4:
            if (Task_TakeFlag(task, 2))
                DEBUG_PRINT();
            if (task->timer > 0x3c || (gPadState[1] & 1)) {
                Menu_EraseRect(0x13, 0xd, 0xc, 2);
                switch (*(s16 *)((u8 *)gGameState + 0x662)) {
                    case 0:
                        Menu_DrawText(0x14, 0xd, gUnk_0824F978);
                        break;
                    case 4:
                        Menu_DrawText(0x14, 0xd, gUnk_0824F984);
                        break;
                    case 5:
                    default:
                        Menu_DrawText(0x14, 0xd, gUnk_0824F990);
                        break;
                }
                Sound_PlaySong(0x167);
                sub081C0CE4_Enter(task, 5);
            }
            break;
        case 5:
            if (Task_TakeFlag(task, 2))
                DEBUG_PRINT();
            if (task->timer > 0x3c) {
                Sub081C0CE4Entry_Show(&task->entries[9]);
                sub081C0CE4_Enter(task, 6);
            }
            break;
        case 6:
            if (Task_TakeFlag(task, 2))
                DEBUG_PRINT();
            if (gPadState[1] & 8) {
                Sound_PlaySong(0xdd);
                sub081C0CE4_Enter(task, 7);
            }
            break;
        case 7:
            wasBit = Task_TakeFlag(task, 2);
            if (wasBit)
                ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xFFFF, 0);
            if (task->timer > 0x20)
                sub081C0CE4_Enter(task, 0x63);
            break;
        case 0x63:
            if (Task_TakeFlag(task, 2))
                DEBUG_PRINT();
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
