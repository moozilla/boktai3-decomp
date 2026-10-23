#include "global.h"
#include "monster_book.h"

s32 Bestiary_GetCategoryFirstEntry(u32);
s32 Bestiary_GetCategoryEntryCount(u32);
void Anim_DrawDecimalDigits(void *, void *, int, int, int, int, int, int, int);

static inline void Sprite_Show(struct BookSprite *s)
{
    s->flags &= ~1;
}

static inline void MonsterBook_HideRows(struct MonsterBook *p)
{
    s32 i, j;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 2; j++)
            p->s54C[i][j].flags |= 1;
        p->s2AC[i].flags |= 1;
    }
}

static inline void DrawListNumbers(struct MonsterBook *p)
{
    s32 i;
    s32 count, first;

    MonsterBook_HideRows(p);
    count = Bestiary_GetCategoryEntryCount(p->category);
    first = Bestiary_GetCategoryFirstEntry(p->category) + 1;
    for (i = 0; i < count; i++) {
        Anim_DrawDecimalDigits(p->s54C[i], p->animData, first + i, 2, 0, 0x18, i * 16 + 0x28, 8, 0);
        Sprite_Show(&p->s2AC[i]);
        p->s2AC[i].x = 0x28;
        p->s2AC[i].y = i * 16 + 0x28;
    }
}

static inline void DrawDetailNumber(struct MonsterBook *p)
{
    MonsterBook_HideRows(p);
    Anim_DrawDecimalDigits(p->s54C[0], p->animData, p->entry + 1, 2, 0, 0x28, 0x18, 8, 0);
}

s32 ThresholdTrack_ClassifyPosition(u8);
s32 Bestiary_CountUnlockedGroupEntries(s32);
s32 Bestiary_GetGroupEntryCount(s32);
void Anim_DrawDecimalDigits(void *, void *, int, int, int, int, int, int, int);

static inline void Sprite_Hide(struct BookSprite *s)
{
    s->flags |= 1;
}

static inline void MonsterBook_HideGroupDigits(struct MonsterBook *p)
{
    s32 j;

    for (j = 0; j < 2; j++) {
        ((struct BookSprite *)((u8 *)p + 0xA8C))[j].flags |= 1;
        ((struct BookSprite *)((u8 *)p + 0xBAC))[j].flags |= 1;
    }
    Sprite_Hide(&p->sB4C);
}

static inline void DrawGroupCounts(struct MonsterBook *p)
{
    s32 group;

    MonsterBook_HideGroupDigits(p);
    group = ThresholdTrack_ClassifyPosition(p->entry);
    Anim_DrawDecimalDigits(p->sA8C, p->animData, Bestiary_CountUnlockedGroupEntries(group), 2, 0,
                           0x98, 0x16, 8, 0);
    Sprite_Show(&p->sB4C);
    p->sB4C.x = 0xA8;
    p->sB4C.y = 0x16;
    Anim_DrawDecimalDigits(p->sBAC, p->animData, Bestiary_GetGroupEntryCount(group), 2, 0, 0xB0,
                           0x16, 8, 0);
}

s32 Anim_LoadKeyframe(void *, void *, u16, u32);

static inline void MonsterBook_HideCategoryBadge(struct MonsterBook *p)
{
    Sprite_Hide(&p->s18C);
    Sprite_Hide(&p->s1EC);
}

static inline void DrawCategoryBadge(struct MonsterBook *p)
{
    s32 frame, value;
    s32 x, y;

    MonsterBook_HideCategoryBadge(p);
    x = 0;
    y = 0;
    switch (p->category) {
        case 0:
        case 1:
        case 2:
            frame = 0;
            value = p->category;
            x = 0x70;
            y = 0x16;
            break;
        case 3:
        case 4:
            frame = 1;
            value = p->category - 3;
            x = 0x80;
            y = 0x16;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            frame = 2;
            value = p->category - 5;
            x = 0x60;
            y = 0x16;
            break;
        case 11:
        case 12:
            frame = 3;
            value = p->category - 11;
            x = 0x80;
            y = 0x16;
            break;
        case 13:
            frame = 4;
            value = -1;
            break;
        default:
            frame = 0;
            value = 1;
            break;
    }
    Anim_LoadKeyframe(&p->s18C, p->animData, frame + 0x41, 2);
    Sprite_Show(&p->s18C);
    if (value >= 0) {
        Anim_LoadKeyframe(&p->s1EC, p->animData, value + 1, 2);
        Sprite_Show(&p->s1EC);
        p->s1EC.x = x;
        p->s1EC.y = y;
    }
}

void BackgroundPanel_ReleaseHandles(struct MonsterBook *);
s32 Bestiary_GetCategoryFirstEntry(u32);
s32 Bestiary_GetCategoryEntryCount(u32);
s32 TextWindow_SetScript(s32, const u8 *);
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
void TextWindow_Refresh(int, int);
void TextWindow_StartPlayback(s32);
s32 GameState_IsEventFlagSet(s32);

static inline void DrawListWindows(struct MonsterBook *p)
{
    s32 i;
    s32 first, count;

    BackgroundPanel_ReleaseHandles(p);
    first = Bestiary_GetCategoryFirstEntry(p->category);
    count = Bestiary_GetCategoryEntryCount(p->category);
    for (i = 0; i < count; i++) {
        s32 e;
        TextWindow_SetScript(p->windows[i], p->script);
        TextWindow_SetBounds(p->windows[i], 6, i * 2 + 5, 0x14, 2);
        e = first + i;
        if (GameState_IsEventFlagSet(e))
            TextWindow_Refresh(p->windows[i], e);
        else
            TextWindow_Refresh(p->windows[i], 0x53);
        TextWindow_StartPlayback(p->windows[i]);
    }
}

void BestiaryDisplay_Hide(void *);
void BestiaryDisplay_Show(void *);
void BestiaryDisplay_SetEntry(void *, u32);
void BestiaryDisplay_DrawEntryDetails(void *);
void BestiaryFlags_Update(void *);
// confidence=low: redraws the selected monster-book list or detail page.
#define MonsterBook_RefreshPage sub_081D5CE0
void MonsterBook_RefreshPage(struct MonsterBook *p)
{
    switch (p->mode) {
        case 0:
            p->s6C.flags &= ~1;
            p->sCC.flags |= 1;
            p->s12C.flags |= 1;
            p->s24C.flags &= ~1;
            DrawListWindows(p);
            DrawCategoryBadge(p);
            DrawListNumbers(p);
            DrawGroupCounts(p);
            BestiaryDisplay_Hide(p->display);
            break;
        case 1:
            p->s6C.flags |= 1;
            p->sCC.flags &= ~1;
            p->s12C.flags &= ~1;
            Sprite_Show(&p->s24C);
            BackgroundPanel_ReleaseHandles(p);
            MonsterBook_HideCategoryBadge(p);
            BestiaryDisplay_DrawEntryDetails(p);
            DrawDetailNumber(p);
            MonsterBook_HideGroupDigits(p);
            BestiaryDisplay_SetEntry(p->display, p->entry);
            if (GameState_IsEventFlagSet(p->entry))
                BestiaryDisplay_Show(p->display);
            else
                BestiaryDisplay_Hide(p->display);
            BestiaryFlags_Update(p);
            break;
    }
}
