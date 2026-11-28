#include "global.h"
#include "engine/ram.h"
#include "link/lobby/record_menu.h"
#include "scene/areas/area_effect_link.h"
#include "system/game_state/game_state.h"

s32 Div(s32, s32);
u16 *Video_GetWrappedBackgroundMapEntry(u32, u32, u32);
void PanelText_DrawDigits(s32, s32, s32, u32, s32);
void Video_DrawBackgroundRowMarkers(s32, u32);
void AnimSprite_SelectGlyph(void *, void *, u16);
void Sound_PlayTrackedSong(u32);

void TimerDisplay_DrawRecord(struct AreaEffectLinkState *records, s32 x, u32 y, s32 unused,
                             s32 index)
{
    s32 ticks;
    s32 seconds;
    s32 fraction;
    u16 *cell;

    cell = Video_GetWrappedBackgroundMapEntry(3, x + 3, y);
    *cell = 31;
    if (!records)
        Video_DrawBackgroundRowMarkers(x, y);
    if (index < 0)
        index = 0;
    if (index > 1)
        index = 1;
    ticks = records->recordTicks[index];
    if (!ticks) {
        Video_DrawBackgroundRowMarkers(x, y);
    } else {
        if (ticks > 59999) {
            seconds = fraction = 999;
        } else {
            seconds = Div(ticks, 60);
            fraction = Div((ticks - seconds * 60) * 169, 10);
        }
        PanelText_DrawDigits(seconds, 100, x, y, 0);
        PanelText_DrawDigits(fraction, 100, x + 4, y, 0);
        cell = Video_GetWrappedBackgroundMapEntry(3, x + 7, y);
        *cell = 1;
    }
}

static inline s32 Clamp999(s32 x)
{
    if (x > 999)
        x = 999;
    else if (x < 0)
        x = 0;
    return x;
}

static inline s32 LinkSession_GetLocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}

void RecordMenu_DrawRecords(struct RecordMenu *menu)
{
    struct AreaEffectLinkState *records;
    u16 *cell;
    s32 i;

    if (!gLinkSession)
        records = NULL;
    else if (gLinkSession->unavailable != 1)
        records = NULL;
    else
        records = gLinkSession;
    TimerDisplay_DrawRecord(records, 5, 17, menu->selection, 0);
    TimerDisplay_DrawRecord(records, 23, 17, menu->selection, 1);
    cell = Video_GetWrappedBackgroundMapEntry(3, 0, 16);
    for (i = 0; i < 30; i++)
        *cell++ = 1;
    if (records->recordTicks[0] == records->recordTicks[1]) {
        gGameState->linkScoreOtherRows[1]++;
        menu->spriteIds[4] = 0x37;
        menu->spriteIds[5] = 0x37;
        for (i = 4; i <= 5; i++)
            AnimSprite_SelectGlyph(&menu->sprites[i], menu->rowResource, menu->spriteIds[i]);
        menu->sprites[4].x = 0;
        menu->sprites[5].x = 0x90;
    } else if (records->recordTicks[0] < records->recordTicks[1]) {
        if (gLinkSession && gLinkSession->localPlayer == 0)
            gGameState->linkScoreFirstRow++;
        else
            gGameState->linkScoreOtherRows[0]++;
        menu->sprites[4].x = 0;
        menu->sprites[5].x = 0x90;
        if (gLinkSession && gLinkSession->localPlayer == 0)
            Sound_PlayTrackedSong(0xA9);
    } else {
        if (LinkSession_GetLocalPlayer() == 1)
            gGameState->linkScoreFirstRow++;
        else
            gGameState->linkScoreOtherRows[0]++;
        menu->sprites[4].x = 0x90;
        menu->sprites[5].x = 0;
        if (LinkSession_GetLocalPlayer() == 1)
            Sound_PlayTrackedSong(0xA9);
    }
    /* Stored through u16 lvalues, so the state pointer is reloaded after each one. */
    *(u16 *)&gGameState->linkScoreFirstRow = Clamp999(gGameState->linkScoreFirstRow);
    *(u16 *)&gGameState->linkScoreOtherRows[0] = Clamp999(gGameState->linkScoreOtherRows[0]);
    *(u16 *)&gGameState->linkScoreOtherRows[1] = Clamp999(gGameState->linkScoreOtherRows[1]);
}
