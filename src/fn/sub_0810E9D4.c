// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "record_menu.h"
#define TileNumber_GetCell     Video_GetWrappedBackgroundMapEntry
#define TileNumber_DrawDecimal PanelText_DrawDigits
#define TimerDisplay_DrawEmpty Video_DrawBackgroundRowMarkers
struct TimerRecords {
    u8 unknown[0x24c];
    s32 ticks[2];
};
s32 Div(s32, s32);
u32 Video_GetWrappedBackgroundMapEntry(s32, s32, u32);
void PanelText_DrawDigits(s32, s32, s32, u32, s32);
void Video_DrawBackgroundRowMarkers(s32, u32);
void TimerDisplay_DrawRecord(struct TimerRecords *records, s32 x, u32 y, s32 unused, s32 index)
{
    s32 ticks, seconds, fraction;
    {
        register u16 *cell asm("r1") = (u16 *)Video_GetWrappedBackgroundMapEntry(3, x + 3, y);
        u16 value = 31;
        asm("" : "+r"(cell));
        *cell = value;
    }
    if (!records)
        Video_DrawBackgroundRowMarkers(x, y);
    if (index < 0)
        index = 0;
    if (index > 1)
        index = 1;
    ticks = records->ticks[index];
    if (!ticks) {
        Video_DrawBackgroundRowMarkers(x, y);
    } else {
        if (ticks > 59999) {
            fraction = 999;
            seconds = fraction;
        } else {
            seconds = Div(ticks, 60);
            fraction = Div((ticks - seconds * 60) * 169, 10);
        }
        PanelText_DrawDigits(seconds, 100, x, y, 0);
        PanelText_DrawDigits(fraction, 100, x + 4, y, 0);
        {
            register u16 *cell asm("r1") = (u16 *)Video_GetWrappedBackgroundMapEntry(3, x + 7, y);
            u16 value = 1;
            asm("" : "+r"(cell));
            *cell = value;
        }
    }
}
struct RecActor {
    u8 unknown[0x18];
    s16 side;
    u8 pad1a[0x53 - 0x1a];
    u8 flag;
    u8 pad54[0x24c - 0x54];
    u32 ticksA;
    u32 ticksB;
};
struct RecScore {
    u8 unknown[0x7b6];
    s16 wins;
    u8 pad7b8[0x7fa - 0x7b8];
    s16 losses;
    s16 draws;
};
extern struct RecActor *gLinkSession;
extern struct RecScore *gGameState;
void Anim_SelectWidgetGlyph(void *, void *, u16);
void Sound_PlayTrackedSong(u32);
static inline s32 Clamp999(s32 x)
{
    if (x > 999)
        x = 999;
    else if (x < 0)
        x = 0;
    return x;
}
/* The store goes through a plain s16 lvalue (not a struct field), so the score pointer is reloaded after it. */
#define CLAMP_SCORE(field, off) *(s16 *)((u8 *)gGameState + off) = Clamp999(gGameState->field);
static inline s32 ActorSide(void)
{
    struct RecActor *a = gLinkSession;
    if (a == 0)
        return -1;
    return a->side;
}
void RecordMenu_DrawRecords(struct RecordMenu *menu)
{
    struct RecActor *rec;
    u16 *cell;
    s32 i;
    {
        struct RecActor *a = gLinkSession;
        u8 f;
        if (a != 0) {
            f = a->flag;
            rec = a;
            if (f != 1)
                rec = 0;
        } else
            rec = 0;
    }
    TimerDisplay_DrawRecord((struct TimerRecords *)rec, 5, 0x11, menu->selection, 0);
    TimerDisplay_DrawRecord((struct TimerRecords *)rec, 0x17, 0x11, menu->selection, 1);
    cell = (u16 *)Video_GetWrappedBackgroundMapEntry(3, 0, 0x10);
    {
        u16 one = 1;
        for (i = 0x1d; i >= 0; i--)
            *cell++ = one;
    }
    if (rec->ticksA == rec->ticksB) {
        gGameState->draws++;
        menu->spriteIds[4] = 0x37;
        menu->spriteIds[5] = 0x37;
        for (i = 4; i <= 5; i++)
            Anim_SelectWidgetGlyph(&menu->sprites[i], menu->rowResource, menu->spriteIds[i]);
        menu->sprites[4].x = 0;
        menu->sprites[5].x = 0x90;
    } else if (rec->ticksA < rec->ticksB) {
        if (gLinkSession != 0 && gLinkSession->side == 0)
            gGameState->wins++;
        else
            gGameState->losses++;
        menu->sprites[4].x = 0;
        menu->sprites[5].x = 0x90;
        if (gLinkSession != 0 && gLinkSession->side == 0)
            Sound_PlayTrackedSong(0xa9);
    } else {
        s32 side = ActorSide();
        if (side == 1)
            gGameState->wins++;
        else
            gGameState->losses++;
        menu->sprites[4].x = 0x90;
        menu->sprites[5].x = 0;
        side = ActorSide();
        if (side == 1)
            Sound_PlayTrackedSong(0xa9);
    }
    CLAMP_SCORE(wins, 0x7b6)
    CLAMP_SCORE(losses, 0x7fa)
    CLAMP_SCORE(draws, 0x7fc)
}
