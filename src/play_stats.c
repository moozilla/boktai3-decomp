#include "play_stats.h"
#include "monster_particle_random.h"

extern void (*const gUnk_0861215C[])(struct PlayStatsScreen *);

extern const u8 gUnk_0824F934[], gUnk_0824F940[], gUnk_0824F948[], gUnk_0824F954[];

extern const u8 gUnk_0824F960[], gUnk_0824F968[], gUnk_0824F970[], gUnk_0824F978[], gUnk_0824F984[],
    gUnk_0824F990[];

void Menu_EraseRect(u32, u32, u32, u32);

void Menu_DrawText(u32, u32, const u8 *);

void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);

void Bg_SetLayerMap(u32, void *, u16);

void GameState_UpdateCounterBalanceRank(void);

void GameStats_SelectLeadingCounter(void);

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

s32 SceneActorManager_Exists(void)
{
    if (gSceneActorListManager)
        return 1;
    return 0;
}

void PlayStats_SetState(struct PlayStatsScreen *screen, u32 screenId)
{
    u32 flags;
    screen->screen = screenId;
    screen->screenTimer = 0;
    screen->callback = gUnk_0861215C[screenId];
    {
        u32 mask = 1;
        flags = screen->flags;
        flags |= mask;
    }
    screen->state = 0;
    screen->timer = 0;
    {
        u32 mask = 2;
        flags |= mask;
    }
    screen->flags = flags;
}

void PlayStats_RemoveAnimations(struct PlayStatsScreen *screen)
{
    s32 i;
    struct SceneInputWidget *animation = screen->entries;
    for (i = 7; i >= 0; i--) {
        Anim_Remove(animation);
        animation++;
    }
    Anim_Remove(&screen->entries[8]);
    Anim_Remove(&screen->entries[9]);
}

void PlayStats_HideEntryAnimations(struct PlayStatsScreen *screen)
{
    s32 i;
    u32 m = 1;
    u32 *flags = &screen->entries[0].flags;
    for (i = 7; i >= 0; i--) {
        *flags |= m;
        flags = (void *)((u8 *)flags + sizeof(struct SceneInputWidget));
    }
}

void PlayStats_DrawLabels(struct PlayStatsScreen *screen)
{
    Menu_EraseRect(2, 4, 10, 1);
    Menu_DrawText(2, 4, gUnk_0824F934);
    Menu_EraseRect(2, 7, 10, 1);
    Menu_DrawText(2, 7, gUnk_0824F940);
    Menu_EraseRect(2, 10, 10, 1);
    Menu_DrawText(2, 10, gUnk_0824F948);
    Menu_EraseRect(2, 13, 10, 1);
    Menu_DrawText(2, 13, gUnk_0824F954);
}

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
                text[0] = task->timeText[0];
                text[1] = task->timeText[1];
            }
            if (task->timer <= 0x68) {
                const u16 *table = gRandomTable;
                u32 *index = &gRandomIndex;
                text[3] = PlayStats_RandomDigit(table, index) + '0';
                text[4] = PlayStats_RandomDigit(table, index) + '0';
            } else {
                text[3] = task->timeText[3];
                text[4] = task->timeText[4];
            }
            if (task->timer <= 0x95) {
                const u16 *table = gRandomTable;
                u32 *index = &gRandomIndex;
                text[6] = PlayStats_RandomDigit(table, index) + '0';
                text[7] = PlayStats_RandomDigit(table, index) + '0';
                if (task->timer & 1)
                    Sound_PlaySong(0x273);
            } else {
                text[6] = task->timeText[6];
                text[7] = task->timeText[7];
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
                Menu_DrawText(0x14, 4, task->timeText);
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
                void *frames = task->resources;
                const u16 *table = gRandomTable;
                struct SceneInputWidget *entry = task->entries;
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
                Menu_DrawText(0x12, 7, task->valueText);
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
                Menu_DrawText(0x12, 7, task->valueText);
            }
            if (task->timer > 0x3c || (gPadState[1] & 1)) {
                Menu_EraseRect(0x17, 0xa, 8, 2);
                v = gGameState->unk660;
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
                v = (*game)->unk662;
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
                    struct SceneInputWidget *entry = &task->entries[9];
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

u32 PlayStats_Update(struct PlayStatsScreen *screen)
{
    if ((screen->flags & 1) == 0)
        screen->screenTimer++;
    if ((screen->flags & 2) == 0)
        screen->timer++;
    Callback_InvokeActor(screen, screen->callback);
    return 0;
}
