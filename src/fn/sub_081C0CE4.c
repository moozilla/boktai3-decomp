#include "global.h"
#include "game_state.h"
#include "input.h"
#include "engine_globals.h"
#include "monster_particle_random.h"

extern const u8 gUnk_0824F960[];
extern const u8 gUnk_0824F968[];
extern const u8 gUnk_0824F970[];
extern const u8 gUnk_0824F978[];
extern const u8 gUnk_0824F984[];
extern const u8 gUnk_0824F990[];

struct PlayStatsEntry {
    u8 unk0[8];
    u32 flags;
    u8 unkC[0x54];
};

struct PlayStatsScreen {
    u8 unk00[0x18];
    u8 keyframeData[0x40];
    void *layerMap;
    u8 unk5C[4];
    u32 flags;
    void *script;
    u8 unk68[8];
    s32 state;
    s32 timer;
    u32 mapId;
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
    struct PlayStatsEntry entries[10];
    u32 soundCued;
};

static inline u32 PlayStats_TakeFlag(struct PlayStatsScreen *task, u32 mask)
{
    u32 flags = task->flags;
    u32 value = flags & mask;
    if (value)
        value = 1;
    task->flags = flags & ~mask;
    return value;
}

static inline void PlayStats_Enter(struct PlayStatsScreen *task, s32 state)
{
    task->state = state;
    task->timer = 0;
    {
        u32 bit = 2;
        task->flags |= bit;
    }
}

static inline s32 PlayStats_RandomDigit(const u16 *table, u32 *index)
{
    u32 sample, next;
    u32 mask;
    next = *index + 1;
    mask = 1023;
    next &= mask;
    *index = next;
    sample = table[next];
    return Mod(sample, 10);
}

/* Reveal the elapsed time, counters and rankings in stages. */
void PlayStats_UpdateResults(struct PlayStatsScreen *task)
{
    u8 text[12];
    u8 blank[12];
    s32 wasSet;
    s32 wasBit;
    s32 i;
    s32 state;
    s16 v;

    wasSet = PlayStats_TakeFlag(task, 1);
    if (wasSet) {
        Bg_ClearTilemap(0);
        BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
        Bg_ClearTilemap(2);
        BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
        Bg_ClearTilemap(3);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        {
            u32 mapId = 11;
            Bg_SetLayerMap(2, task->layerMap, mapId);
            BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
            task->mapId = mapId;
        }
        GameState_UpdateCounterBalanceRank();
        GameStats_SelectLeadingCounter();
        PlayStats_DrawLabels(task);
        task->state = 0;
        task->timer = 0;
        {
            u32 bit = 2;
            task->flags |= bit;
        }
    }

    state = task->state;
    switch (state) {
        case 0:
            wasBit = PlayStats_TakeFlag(task, 2);
            if (wasBit)
                ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, state);
            if (task->timer > 0x20)
                PlayStats_Enter(task, 1);
            break;
        case 1:
            wasBit = PlayStats_TakeFlag(task, 2);
            if (wasBit)
                task->soundCued = 0;
            text[2] = text[5] = ':';
            if (task->timer <= 0x3b) {
                const u16 *table = gRandomTable;
                u32 *index = &gRandomIndex;
                text[0] = PlayStats_RandomDigit(table, index) + '0';
                text[1] = PlayStats_RandomDigit(table, index) + '0';
            } else {
                text[0] = task->digit7C;
                text[1] = task->digit7D;
            }
            if (task->timer <= 0x68) {
                const u16 *table = gRandomTable;
                u32 *index = &gRandomIndex;
                text[3] = PlayStats_RandomDigit(table, index) + '0';
                text[4] = PlayStats_RandomDigit(table, index) + '0';
            } else {
                text[3] = task->digit7F;
                text[4] = task->digit80;
            }
            if (task->timer <= 0x95) {
                const u16 *table = gRandomTable;
                u32 *index = &gRandomIndex;
                text[6] = PlayStats_RandomDigit(table, index) + '0';
                text[7] = PlayStats_RandomDigit(table, index) + '0';
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
                PlayStats_Enter(task, 2);
            }
            break;
        case 2:
            wasBit = PlayStats_TakeFlag(task, 2);
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
                u8 *frames = task->keyframeData;
                const u16 *table = gRandomTable;
                struct PlayStatsEntry *entry = task->entries;
                u32 *cursor = &gRandomIndex;
                for (i = 7; i >= 0; i--) {
                    u32 random;
                    *cursor = (*cursor + 1) & 1023;
                    PARTICLE_RANDOM_READ_SAMPLE(random, table, *cursor);
                    Anim_LoadKeyframe(entry, frames, (u16)Mod(random, 10), 1);
                    entry->flags &= ~1;
                    entry++;
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
                PlayStats_Enter(task, 3);
            }
            if (gPadState[1] & 1) {
                if (!task->soundCued) {
                    Sound_PlaySong(0x167);
                    task->soundCued = 1;
                }
                PlayStats_Enter(task, 3);
            }
            break;
        case 3:
            wasBit = PlayStats_TakeFlag(task, 2);
            if (wasBit) {
                PlayStats_HideEntryAnimations(task);
                Menu_EraseRect(0x12, 7, 0xa, 2);
                Menu_DrawText(0x12, 7, task->label85);
            }
            if (task->timer > 0x3c || (gPadState[1] & 1)) {
                Menu_EraseRect(0x17, 0xa, 8, 2);
                v = *(s16 *)((u8 *)gGameState + 0x660);
                switch (v) {
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
                PlayStats_Enter(task, 4);
            }
            break;
        case 4:
            PlayStats_TakeFlag(task, 2);
            if (task->timer > 0x3c || (gPadState[1] & 1)) {
                struct GameState **game = &gGameState;
                Menu_EraseRect(0x13, 0xd, 0xc, 2);
                v = *(s16 *)((u8 *)*game + 0x662);
                switch (v) {
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
                PlayStats_Enter(task, 5);
            }
            break;
        case 5:
            PlayStats_TakeFlag(task, 2);
            if (task->timer > 0x3c) {
                {
                    struct PlayStatsEntry *entry = &task->entries[9];
                    entry->flags &= ~1;
                }
                PlayStats_Enter(task, 6);
            }
            break;
        case 6:
            PlayStats_TakeFlag(task, 2);
            if (gPadState[1] & 8) {
                Sound_PlaySong(0xdd);
                PlayStats_Enter(task, 7);
            }
            break;
        case 7:
            wasBit = PlayStats_TakeFlag(task, 2);
            if (wasBit)
                ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xFFFF, 0);
            if (task->timer > 0x20)
                PlayStats_Enter(task, 0x63);
            break;
        case 0x63:
            PlayStats_TakeFlag(task, 2);
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
