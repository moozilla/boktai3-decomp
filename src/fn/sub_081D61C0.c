// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "monster_book.h"
#include "input.h"

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

void Sound_PlaySong(u32);
void MonsterBook_SetPage(void *, u32);
void Script_Call(void *, u32);
void ThresholdTrack_SelectInterval(void *);
void MonsterBook_UpdateHighlightPulse(void *);
void ScrollingDialogue_Step(void *);
#define BOOK_FIELD(p, off, type) (*(type *)((u8 *)(p) + (off)))
// confidence=low: processes list/detail navigation and page-close input.
#define MonsterBook_UpdateNavigation sub_081D61C0
u32 MonsterBook_UpdateNavigation(struct MonsterBook *p)
{
    u32 changed = 0;
    u32 moved = 0;
    // Preserve the compiler PRE table ordering for the long-lived page pointers.
    asm("");
    asm("");
    asm("");
    asm("");
    asm("");
    switch (p->mode) {
        case 0:
            if (gPadState[1] & 0x200) {
                if (p->category)
                    p->category--;
                else
                    p->category = 13;
                Sound_PlaySong(0x10A);
                goto categoryChanged;
            } else if (gPadState[1] & 0x100) {
                if (p->category <= 12)
                    p->category++;
                else
                    p->category = 0;
                Sound_PlaySong(0x10A);
                moved = 1;
            }
            if (moved) {
            categoryChanged:
                p->entry = Bestiary_GetCategoryFirstEntry(p->category);
                BOOK_FIELD(p, 0x48, u8) = 0;
                changed = 1;
                DrawListWindows(p);
                DrawCategoryBadge(p);
                DrawListNumbers(p);
                DrawGroupCounts(p);
                BestiaryFlags_Update(p);
            }
            if ((gPadState[1] & 0x40) && p->entry) {
                u8 *selection = (u8 *)p + 0x48;
                if (*selection) {
                    p->entry--;
                    (*selection)--;
                } else {
                    *selection = Bestiary_GetCategoryEntryCount(p->category) - 1;
                    p->entry = Bestiary_GetCategoryFirstEntry(p->category) + *selection;
                }
                Sound_PlaySong(0xDC);
                goto selectionChanged;
            } else if ((gPadState[1] & 0x80) && p->entry <= 0x51) {
                u8 *selection = (u8 *)p + 0x48;
                s32 current = *selection;
                if (current < Bestiary_GetCategoryEntryCount(p->category) - 1) {
                    p->entry++;
                    (*selection)++;
                } else {
                    p->entry = Bestiary_GetCategoryFirstEntry(p->category);
                    *selection = 0;
                }
                Sound_PlaySong(0xDC);
                changed = 1;
            }
            if (changed) {
            selectionChanged:
                p->s6C.y = BOOK_FIELD(p, 0x48, u8) << 4;
            }
            if (gPadState[1] & 1) {
                MonsterBook_SetPage(p, 1);
                BestiaryFlags_Update(p);
                Sound_PlaySong(0xDD);
            }
            if (gPadState[1] & 2) {
                void *script = BOOK_FIELD(p, 0xF18, void *);
                if (script)
                    Script_Call(script, 0);
                Sound_PlaySong(0x119);
                p->mode = 2;
            }
            break;
        case 1:
            if (gPadState[1] & 0x40) {
                if (p->unk27)
                    p->unk27--;
                else
                    p->unk27 = 3;
                Sound_PlaySong(0xDC);
                goto tabChanged;
            } else if (gPadState[1] & 0x80) {
                if (p->unk27 <= 2)
                    p->unk27++;
                else
                    p->unk27 = 0;
                Sound_PlaySong(0xDC);
                changed = 1;
            }
            if (changed) {
            tabChanged:
                p->sCC.y = p->unk27 << 4;
                BestiaryDisplay_DrawEntryDetails(p);
            }
            if (gPadState[1] & 0x100) {
                if (p->entry <= 0x51)
                    p->entry++;
                else
                    p->entry = 0;
                Sound_PlaySong(0x10A);
                goto entryChanged;
            } else if (gPadState[1] & 0x200) {
                if (p->entry)
                    p->entry--;
                else
                    p->entry = 0x52;
                Sound_PlaySong(0x10A);
                moved = 1;
            }
            if (moved) {
            entryChanged:
                BestiaryDisplay_DrawEntryDetails(p);
                DrawDetailNumber(p);
                if (GameState_IsEventFlagSet(p->entry)) {
                    BestiaryDisplay_Show(p->display);
                    BestiaryDisplay_SetEntry(p->display, p->entry);
                } else
                    BestiaryDisplay_Hide(p->display);
                BestiaryFlags_Update(p);
            }
            {
                u8 *pending = (u8 *)p + 0xDA7;
                switch (*pending) {
                    case 0:
                        if (gPadState[1] & 2) {
                            BOOK_FIELD(p, 0xD4E, u16) |= 1;
                            ThresholdTrack_SelectInterval(p);
                            p->s6C.y = BOOK_FIELD(p, 0x48, u8) << 4;
                            *pending = 1;
                        }
                        break;
                    default:
                        MonsterBook_SetPage(p, 0);
                        BestiaryFlags_Update(p);
                        Sound_PlaySong(0xDE);
                        break;
                }
                asm("");
            }
            break;
        case 2:
            return 0;
    }
    BestiaryFlags_Update(p);
    MonsterBook_UpdateHighlightPulse(p);
    ScrollingDialogue_Step(p);
    BOOK_FIELD(p, 0x20, u32)++;
    return 0;
}
