#include "ui/books/monster_book.h"
#include "gba/io_reg.h"
#include "ui/books/bestiary.h"

static inline void WidgetPos_Clear(struct ContactPosition *p)
{
    p->x = 0;
    p->y = 0;
    p->z = 0;
    p->pad = 0;
}

static inline void MonsterBook_HideRows(struct MonsterBook *p)
{
    s32 i, j;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 2; j++)
            p->entryDigits[i][j].flags |= 1;
        p->rowMarkers[i].flags |= 1;
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
        Anim_DrawDecimalDigits(p->entryDigits[i], &p->resource, first + i, 2, 0, 0x18,
                               i * 16 + 0x28, 8, 0);
        AnimSprite_Show(&p->rowMarkers[i]);
        p->rowMarkers[i].position.x = 0x28;
        p->rowMarkers[i].position.y = i * 16 + 0x28;
    }
}

static inline void DrawDetailNumber(struct MonsterBook *p)
{
    MonsterBook_HideRows(p);
    Anim_DrawDecimalDigits(p->entryDigits[0], &p->resource, p->entry + 1, 2, 0, 0x28, 0x18, 8, 0);
}

static inline void MonsterBook_HideGroupDigits(struct MonsterBook *p)
{
    s32 j;

    for (j = 0; j < 2; j++) {
        (&p->unlockedDigits[0])[j].flags |= 1;
        (&p->totalDigits[0])[j].flags |= 1;
    }
    AnimSprite_Hide(&p->countSeparator);
}

static inline void DrawGroupCounts(struct MonsterBook *p)
{
    s32 group;

    MonsterBook_HideGroupDigits(p);
    group = Bestiary_GetEntryGroup(p->entry);
    Anim_DrawDecimalDigits(p->unlockedDigits, &p->resource,
                           Bestiary_CountUnlockedGroupEntries(group), 2, 0, 0x98, 0x16, 8, 0);
    AnimSprite_Show(&p->countSeparator);
    p->countSeparator.position.x = 0xA8;
    p->countSeparator.position.y = 0x16;
    Anim_DrawDecimalDigits(p->totalDigits, &p->resource, Bestiary_GetGroupEntryCount(group), 2, 0,
                           0xB0, 0x16, 8, 0);
}

static inline void MonsterBook_HideCategoryBadge(struct MonsterBook *p)
{
    AnimSprite_Hide(&p->categoryBadge);
    AnimSprite_Hide(&p->categoryNumber);
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
    Anim_LoadKeyframe(&p->categoryBadge, &p->resource, (u16)(frame + 0x41), 2);
    AnimSprite_Show(&p->categoryBadge);
    if (value >= 0) {
        Anim_LoadKeyframe(&p->categoryNumber, &p->resource, (u16)(value + 1), 2);
        AnimSprite_Show(&p->categoryNumber);
        p->categoryNumber.position.x = x;
        p->categoryNumber.position.y = y;
    }
}

static inline void DrawListWindows(struct MonsterBook *p)
{
    s32 i;
    s32 first, count;

    MonsterBook_ClearWindows(p);
    first = Bestiary_GetCategoryFirstEntry(p->category);
    count = Bestiary_GetCategoryEntryCount(p->category);
    for (i = 0; i < count; i++) {
        s32 e;
        TextWindow_SetScript(p->windows[i], p->script);
        TextWindow_SetBounds(p->windows[i], 6, i * 2 + 5, 0x14, 2);
        e = first + i;
        if (Bestiary_IsEntryUnlocked(e))
            TextWindow_Refresh(p->windows[i], e);
        else
            TextWindow_Refresh(p->windows[i], 0x53);
        TextWindow_StartPlayback(p->windows[i]);
    }
}

static inline u8 MonsterBook_CurrentEntry(struct MonsterBook *owner)
{
    return owner->entry;
}

void MonsterBook_ClearWindows(struct MonsterBook *book)
{
    s32 i;

    for (i = 0; i < 8; i++)
        TextWindow_Clear(book->windows[i]);
    book->transitionFlags |= 1;
    Bg_ClearTilemap(0);
}

void MonsterBook_DrawEntryDetails(struct MonsterBook *book)
{
    s32 unlocked = Bestiary_IsEntryUnlocked(book->entry);
    s32 rank;
    struct MonsterBookReward *reward;
    s32 y;
    MonsterBook_ClearWindows(book);
    TextWindow_SetScript(book->windows[0], book->script);
    TextWindow_SetBounds(book->windows[0], 8, 3, 20, 2);
    if (Bestiary_IsEntryUnlocked(book->entry))
        TextWindow_Refresh(book->windows[0], book->entry);
    else
        TextWindow_Refresh(book->windows[0], 83);
    TextWindow_StartPlayback(book->windows[0]);
    switch (book->detailTab) {
        case 0:
            if (unlocked) {
                rank = Bestiary_GetEntryGroup(book->entry);
                TextWindow_SetScript(book->windows[1], book->rankScript);
                TextWindow_SetBounds(book->windows[1], 1, 16, 30, 4);
                TextWindow_Refresh(book->windows[1], rank);
                TextWindow_StartPlayback(book->windows[1]);
            }
            break;
        case 1:
            if (unlocked) {
                TextWindow_SetScript(book->windows[2], book->countScript);
                TextWindow_SetBounds(book->windows[2], 1, 16, 30, 4);
                TextWindow_Refresh(book->windows[2], book->counts[book->entry]);
                TextWindow_StartPlayback(book->windows[2]);
            }
            break;
        case 2:
            if (unlocked)
                book->transitionFlags = (book->transitionFlags & ~1) | 8;
            else {
                book->transitionFlags &= ~1;
                ScrollingDialogue_ClearScript();
            }
            break;
        case 3:
            if (!unlocked)
                break;
            reward = &book->rewards[book->entry];
            y = 16;
            TextWindow_SetScript(book->windows[6], book->labelScript);
            TextWindow_SetBounds(book->windows[6], 1, y, 6, 2);
            TextWindow_Refresh(book->windows[6], 0);
            TextWindow_StartPlayback(book->windows[6]);
            if (reward->firstId == 255) {
                TextWindow_SetScript(book->windows[4], book->emptyScript);
                TextWindow_SetBounds(book->windows[4], 6, y, 20, 2);
                TextWindow_Refresh(book->windows[4], 0);
                TextWindow_StartPlayback(book->windows[4]);
            } else if (Bestiary_IsRewardUnlocked(book->entry, 0)) {
                if (reward->firstId <= 254) {
                    switch (reward->firstKind) {
                        case 0:
                            TextWindow_SetScript(book->windows[4], book->rewardScript);
                            TextWindow_SetBounds(book->windows[4], 6, y, 20, 2);
                            TextWindow_Refresh(book->windows[4], reward->firstId);
                            TextWindow_StartPlayback(book->windows[4]);
                            break;
                        case 1:
                            TextWindow_SetScript(book->windows[4], book->nameScript);
                            TextWindow_SetBounds(book->windows[4], 6, y, 20, 2);
                            TextWindow_Refresh(book->windows[4], reward->firstId);
                            TextWindow_SetString(book->windows[4], 0, book->playerName);
                            TextWindow_StartPlayback(book->windows[4]);
                            break;
                        case 2:
                            TextWindow_SetScript(book->windows[4], book->itemScript);
                            TextWindow_SetBounds(book->windows[4], 6, y, 20, 2);
                            TextWindow_Refresh(book->windows[4], reward->firstId);
                            TextWindow_StartPlayback(book->windows[4]);
                            break;
                    }
                }
            } else {
                TextWindow_SetScript(book->windows[4], book->script);
                TextWindow_SetBounds(book->windows[4], 6, y, 20, 2);
                TextWindow_Refresh(book->windows[4], 83);
                TextWindow_StartPlayback(book->windows[4]);
            }
            y += 2;
            TextWindow_SetScript(book->windows[7], book->labelScript);
            TextWindow_SetBounds(book->windows[7], 1, y, 6, 2);
            TextWindow_Refresh(book->windows[7], 1);
            TextWindow_StartPlayback(book->windows[7]);
            if (reward->secondId == 255) {
                TextWindow_SetScript(book->windows[5], book->emptyScript);
                TextWindow_SetBounds(book->windows[5], 6, y, 30, 2);
                TextWindow_Refresh(book->windows[5], 0);
                TextWindow_StartPlayback(book->windows[5]);
            } else if (Bestiary_IsRewardUnlocked(book->entry, 1)) {
                if (reward->secondId <= 254) {
                    switch (reward->secondKind) {
                        case 0:
                            TextWindow_SetScript(book->windows[5], book->rewardScript);
                            TextWindow_SetBounds(book->windows[5], 6, y, 30, 2);
                            TextWindow_Refresh(book->windows[5], reward->secondId);
                            TextWindow_StartPlayback(book->windows[5]);
                            break;
                        case 1:
                            TextWindow_SetScript(book->windows[5], book->nameScript);
                            TextWindow_SetBounds(book->windows[5], 6, y, 30, 2);
                            TextWindow_Refresh(book->windows[5], reward->secondId);
                            TextWindow_SetString(book->windows[5], 0, book->playerName);
                            TextWindow_StartPlayback(book->windows[5]);
                            break;
                        case 2:
                            TextWindow_SetScript(book->windows[5], book->itemScript);
                            TextWindow_SetBounds(book->windows[5], 6, y, 30, 2);
                            TextWindow_Refresh(book->windows[5], reward->secondId);
                            TextWindow_StartPlayback(book->windows[5]);
                            break;
                    }
                }
            } else {
                TextWindow_SetScript(book->windows[5], book->script);
                TextWindow_SetBounds(book->windows[5], 6, y, 30, 2);
                TextWindow_Refresh(book->windows[5], 83);
                TextWindow_StartPlayback(book->windows[5]);
            }
            break;
    }
}

void MonsterBook_UpdateDescription(struct MonsterBook *book)
{
    if (book->transitionFlags & 1) {
        if (book->transitionFlags & 2) {
            ScrollingDialogue_ClearScript();
            ScrollingDialogue_HideImmediately();
            book->transitionFlags = (book->transitionFlags & ~2) | 4;
        }
    } else if (book->transitionFlags & 8) {
        if (book->transitionFlags & 4) {
            ScrollingDialogue_ClearScript();
            ScrollingDialogue_RequestShow();
            book->transitionFlags &= ~4;
        }
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_SetScript(
            Text_LookupString(Script_ParseStringRef(book->dialogueScript) + book->entry));
        book->transitionFlags = (book->transitionFlags & ~8) | 2;
        book->transitionFlags &= ~4;
    }
}

void MonsterBook_UpdateHighlightPulse(struct MonsterBook *p)
{
    s16 frame = p->highlightFrame;
    u32 color;

    if (frame <= 9)
        color = 0x1f;
    else if (frame <= 0x11)
        color = 0x1b;
    else if (frame <= 0x19)
        color = 0x12;
    else if (frame <= 0x21)
        color = 10;
    else if (frame <= 0x29)
        color = 0x12;
    else
        color = 0x1b;
    p->highlightFrame++;
    if (p->highlightFrame > 0x31)
        p->highlightFrame = 0;
    p->palette[12] = color;
}

u32 MonsterBook_InitWidgets(struct MonsterBook *book)
{
    struct ContactPosition position;
    struct AnimSpriteResource *resource;
    s32 i, j;

    resource = ScriptProc_Start(0xCB05, 0xE2AB);
    book->resource = *resource;
    ResourceHeader_Relocate(&book->resource, resource);
    WidgetPos_Clear(&position);
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 2; j++)
            AnimSprite_InitLinked(&book->entryDigits[i][j], &book->resource, 0, 0x10, 0, 2, 0x3C,
                              &position);
        AnimSprite_InitLinked(&book->rowMarkers[i], &book->resource, 0x14, 0x10, 0, 2, 0x3C, &position);
    }
    WidgetPos_Clear(&position);
    for (i = 0; i < 2; i++) {
        AnimSprite_InitLinked(&book->unlockedDigits[i], &book->resource, 0, 0x10, 0, 2, 0x3C,
                          &position);
        AnimSprite_InitLinked(&book->totalDigits[i], &book->resource, 0, 0x10, 0, 2, 0x3C, &position);
    }
    AnimSprite_InitLinked(&book->countSeparator, &book->resource, 0x15, 0x10, 0, 2, 0x3C, &position);
    WidgetPos_Clear(&position);
    AnimSprite_InitLinked(&book->completionBadge, &book->resource, 0x3E, 0x10, 0, 2, 0x3C,
                          &position);
    AnimSprite_Hide(&book->completionBadge);
    WidgetPos_Clear(&position);
    AnimSprite_InitLinked(&book->groupBadge, &book->resource, 0x40, 0x10, 0, 2, 0x3C, &position);
    book->groupBadge.flags |= 1;
    WidgetPos_Clear(&position);
    AnimSprite_InitLinked(&book->titleIcon, &book->resource, 0x3F, 0x10, 0, 2, 0x3C, &position);
    WidgetPos_Clear(&position);
    AnimSprite_InitLinked(&book->detailMarker, &book->resource, 0x48, 0x10, 0, 2, 0x3C, &position);
    WidgetPos_Clear(&position);
    AnimSprite_InitLinked(&book->categoryNumber, &book->resource, 1, 0x10, 0, 2, 0x3C, &position);
    WidgetPos_Clear(&position);
    AnimSprite_InitLinked(&book->categoryBadge, &book->resource, 0x41, 0x10, 0, 2, 0x3C, &position);
    WidgetPos_Clear(&position);
    AnimSprite_InitLinked(&book->selectionCursor, &book->resource, 0x46, 0x10, 0, 2, 0x3C, &position);
    book->selectionCursor.position.y = book->selectedRow << 4;
    WidgetPos_Clear(&position);
    AnimSprite_InitLinked(&book->tabCursor, &book->resource, 0x47, 0x10, 0, 2, 0x3C, &position);
    CpuSet(book->selectionCursor.palette, book->palette, 0x04000008);
    book->selectionCursor.palette = book->palette;
    book->tabCursor.palette = book->palette;
}

u32 MonsterBook_SetupPageBackgrounds(struct MonsterBook *book, u32 value)
{
    u16 *palette;
    struct BackgroundLayer *layer;
    s32 i;
    u32 dispcnt = DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_BG3_ON;
    u32 white;

    dispcnt |= gDispCntShadow;
    gDispCntShadow = dispcnt;
    book->backgroundResource = ScriptProc_Start(0xC091, 0x3536);
    Bg_SetupLayers(1, 0, book->backgroundResource, 0, 0, 1, &value);
    Bg_SetPriority(2, 0);
    BgGfx_LoadTilemapAtPosition(2, 0, 0);
    book->paletteSource = ((struct MonsterBookPalette *)ScriptProc_Start(0x92B3, 0x204))->colors;
    palette = (u16 *)gBgPaletteBuffer;
    CpuSet(book->paletteSource, palette, 0x30);
    white = 0x73FF;
    for (i = 0; i < 16; i++)
        palette[0x40 + i] = white;
    Bg_SetupLayers(2, 0, book->backgroundResource, 0, 0, 1, &value);
    layer = &gBgRecords[3];
    layer->width = 0x10;
    layer->x = 0;
    layer->y = 0;
    Bg_SetPriority(3, 3);
    Bg_FillTilemapRectangle(3, 4, 6, 8, 9, 1, 0, 4);
    return 0;
}

void MonsterBook_RefreshPage(struct MonsterBook *book)
{
    switch (book->mode) {
        case 0:
            book->selectionCursor.flags &= ~1;
            book->tabCursor.flags |= 1;
            book->detailMarker.flags |= 1;
            book->titleIcon.flags &= ~1;
            DrawListWindows(book);
            DrawCategoryBadge(book);
            DrawListNumbers(book);
            DrawGroupCounts(book);
            BestiaryDisplay_Hide(book->display);
            break;
        case 1:
            book->selectionCursor.flags |= 1;
            book->tabCursor.flags &= ~1;
            book->detailMarker.flags &= ~1;
            AnimSprite_Show(&book->titleIcon);
            MonsterBook_ClearWindows(book);
            MonsterBook_HideCategoryBadge(book);
            MonsterBook_DrawEntryDetails(book);
            DrawDetailNumber(book);
            MonsterBook_HideGroupDigits(book);
            BestiaryDisplay_SetEntry(book->display, book->entry);
            if (Bestiary_IsEntryUnlocked(book->entry))
                BestiaryDisplay_Show(book->display);
            else
                BestiaryDisplay_Hide(book->display);
            Bestiary_UpdateCompletionBadges(book);
            break;
    }
}

void MonsterBook_SetPage(struct MonsterBook *book, u8 mode)
{
    book->mode = mode;
    book->pendingExit = 0;
    MonsterBook_SetupPageBackgrounds(book, book->mode + 0xd);
    MonsterBook_RefreshPage(book);
}

u32 MonsterBook_UpdateNavigation(struct MonsterBook *book)
{
    u32 changed = 0;
    u32 moved = 0;
    switch (book->mode) {
        case 0:
            if (gPadState[1] & L_BUTTON) {
                if (book->category)
                    book->category--;
                else
                    book->category = 13;
                Sound_PlaySong(0x10A);
                goto categoryChanged;
            } else if (gPadState[1] & R_BUTTON) {
                if (book->category <= 12)
                    book->category++;
                else
                    book->category = 0;
                Sound_PlaySong(0x10A);
                moved = 1;
            }
            if (moved) {
            categoryChanged:
                book->entry = Bestiary_GetCategoryFirstEntry(book->category);

                book->selectedRow = 0;
                changed = 1;
                DrawListWindows(book);
                DrawCategoryBadge(book);
                DrawListNumbers(book);
                DrawGroupCounts(book);
                Bestiary_UpdateCompletionBadges(book);
            }
            if ((gPadState[1] & DPAD_UP) && MonsterBook_CurrentEntry(book)) {
                u8 *selection = &book->selectedRow;
                if (*selection) {
                    book->entry--;
                    (*selection)--;
                } else {
                    *selection = Bestiary_GetCategoryEntryCount(book->category) - 1;
                    book->entry = Bestiary_GetCategoryFirstEntry(book->category) + (*selection);
                }
                Sound_PlaySong(0xDC);
                goto selectionChanged;
            } else if ((gPadState[1] & DPAD_DOWN) && (MonsterBook_CurrentEntry(book) <= 0x51)) {
                u8 *selection = &book->selectedRow;
                s32 current = *selection;
                if (current < (Bestiary_GetCategoryEntryCount(book->category) - 1)) {
                    book->entry++;
                    (*selection)++;
                } else {
                    book->entry = Bestiary_GetCategoryFirstEntry(book->category);
                    *selection = 0;
                }
                Sound_PlaySong(0xDC);
                changed = 1;
            }
            if (changed) {
            selectionChanged:
                book->selectionCursor.position.y = book->selectedRow << 4;
            }
            if (gPadState[1] & A_BUTTON) {
                MonsterBook_SetPage(book, 1);
                Bestiary_UpdateCompletionBadges(book);
                Sound_PlaySong(0xDD);
            }
            if (gPadState[1] & B_BUTTON) {
                void *script = book->closeScript;
                if (script)
                    Script_Call(script, 0);
                Sound_PlaySong(0x119);
                book->mode = 2;
            }
            break;

        case 1:
            if (gPadState[1] & DPAD_UP) {
                if (book->detailTab)
                    book->detailTab--;
                else
                    book->detailTab = 3;
                Sound_PlaySong(0xDC);
                goto tabChanged;
            } else if (gPadState[1] & DPAD_DOWN) {
                if (book->detailTab <= 2)
                    book->detailTab++;
                else
                    book->detailTab = 0;
                Sound_PlaySong(0xDC);
                changed = 1;
            }
            if (changed) {
            tabChanged:
                book->tabCursor.position.y = book->detailTab << 4;

                MonsterBook_DrawEntryDetails(book);
            }
            if (gPadState[1] & R_BUTTON) {
                if (MonsterBook_CurrentEntry(book) <= 0x51)
                    book->entry++;
                else
                    book->entry = 0;
                Sound_PlaySong(0x10A);
                goto entryChanged;
            } else if (gPadState[1] & L_BUTTON) {
                if (MonsterBook_CurrentEntry(book))
                    book->entry--;
                else
                    book->entry = 0x52;
                Sound_PlaySong(0x10A);
                moved = 1;
            }
            if (moved) {
            entryChanged:
                MonsterBook_DrawEntryDetails(book);

                DrawDetailNumber(book);
                if (Bestiary_IsEntryUnlocked(MonsterBook_CurrentEntry(book))) {
                    BestiaryDisplay_Show(book->display);
                    BestiaryDisplay_SetEntry(book->display, MonsterBook_CurrentEntry(book));
                } else
                    BestiaryDisplay_Hide(book->display);
                Bestiary_UpdateCompletionBadges(book);
            }
            {
                u8 *pending = &book->pendingExit;
                switch (*pending) {
                    default:
                        MonsterBook_SetPage(book, 0);
                        Bestiary_UpdateCompletionBadges(book);
                        Sound_PlaySong(0xDE);
                        break;

                    case 0:
                        if (gPadState[1] & B_BUTTON) {
                            u16 *flags = &book->transitionFlags;
                            *flags |= 1;
                            Bestiary_SelectCategoryForEntry(book);
                            book->selectionCursor.position.y = book->selectedRow << 4;
                            *pending = 1;
                        }
                        break;
                }
            }
            break;

        case 2:
            return 0;
    }

    Bestiary_UpdateCompletionBadges(book);
    MonsterBook_UpdateHighlightPulse(book);
    MonsterBook_UpdateDescription(book);
    book->frame++;
    return 0;
}

void MonsterBook_RemoveActiveAnimation(struct AnimSprite *p)
{
    if (p->active != 0) {
        Anim_Remove(p);
    }
}

s32 MonsterBook_Destroy(struct MonsterBook *book)
{
    s32 i;
    MonsterBook_RemoveActiveAnimation(&book->tabCursor);
    MonsterBook_RemoveActiveAnimation(&book->selectionCursor);
    MonsterBook_RemoveActiveAnimation(&book->tabCursor);
    MonsterBook_RemoveActiveAnimation(&book->detailMarker);
    MonsterBook_RemoveActiveAnimation(&book->categoryBadge);
    MonsterBook_RemoveActiveAnimation(&book->categoryNumber);
    MonsterBook_RemoveActiveAnimation(&book->titleIcon);
    for (i = 0; i < 7; i++) {
        MonsterBook_RemoveActiveAnimation(&book->rowMarkers[i]);
        MonsterBook_RemoveActiveAnimation(&book->entryDigits[i][0]);
        MonsterBook_RemoveActiveAnimation(&book->entryDigits[i][1]);
    }
    for (i = 0; i < 2; i++) {
        MonsterBook_RemoveActiveAnimation(&book->unlockedDigits[i]);
        MonsterBook_RemoveActiveAnimation(&book->totalDigits[i]);
    }
    MonsterBook_RemoveActiveAnimation(&book->countSeparator);
    MonsterBook_RemoveActiveAnimation(&book->completionBadge);
    MonsterBook_RemoveActiveAnimation(&book->groupBadge);
    return 0;
}

s32 MonsterBook_Init(struct MonsterBook *book, void *context)
{
    s32 i;
    s32 j;
    u8 *src;

    book->frame = 0;
    book->mode = 0;
    book->category = 0;
    book->entry = 0;
    book->selectedRow = 0;
    book->highlightFrame = 0;
    book->playerName[0] = 0x20;
    book->playerName[1] = 0;
    if (Script_SeekToKeyword('n') == 0)
        return -1;
    if ((book->script = Script_GetTextSourcePc()) == 0)
        return -1;
    for (j = 0; j < 8; j++) {
        book->windows[j] = TextWindow_Create(3, j * 2 + 5, 0x17, 2);
        TextWindow_SetScript(book->windows[j], book->script);
        TextWindow_Refresh(book->windows[j], j);
        TextWindow_Clear(book->windows[j]);
    }
    if (Script_SeekToKeyword('d')) {
        Script_SetPc(sub_08227E9C());
        for (i = 0; i < 83; i++)
            book->counts[i] = Script_GetValue();
    }
    if (Script_SeekToKeyword('t')) {
        src = sub_08227E9C();
        for (i = 0; i <= 0x52; i++) {
            book->rewards[i].firstKind = *src++;
            book->rewards[i].firstId = *src++;
            book->rewards[i].secondKind = *src++;
            book->rewards[i].secondId = *src++;
        }
    }
    if (Script_SeekToKeyword('i'))
        book->rewardScript = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('s'))
        book->emptyScript = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('p'))
        book->labelScript = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('w'))
        book->nameScript = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('A'))
        book->itemScript = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('a'))
        book->countScript = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('m'))
        book->dialogueScript = Script_GetTextSourcePc();
    if (Script_SeekToKeyword('T'))
        book->rankScript = Script_GetTextSourcePc();
    book->closeScript = Script_GetKeywordValue('e', 0);
    book->transitionFlags = 5;
    ScrollingDialogue_ClearScript();
    book->display = BestiaryDisplay_Create(0, 0);
    BestiaryDisplay_Hide(book->display);
    MonsterBook_InitWidgets(book);
    MonsterBook_SetPage(book, 0);
    ScreenTransition_SendCommand(0, 4, 4, 4, 4, 0xFFFF, 0);
    Sound_PlaySong(0x118);
    return 0;
}

struct MonsterBook *MonsterBook_Create(void *a)
{
    struct MonsterBook *p = Task_Create(8, sizeof(*p));
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))MonsterBook_UpdateNavigation,
                          (void (*)(void *))MonsterBook_Destroy);
        if (MonsterBook_Init(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

void MonsterBook_HideEntryRows(struct MonsterBook *p)
{
    s32 i, j;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 2; j++)
            p->entryDigits[i][j].flags |= 1;
        p->rowMarkers[i].flags |= 1;
    }
}

void MonsterBook_DrawListNumbers(struct MonsterBook *p)
{
    s32 i;
    s32 count, first;

    MonsterBook_HideRows(p);
    count = Bestiary_GetCategoryEntryCount(p->category);
    first = Bestiary_GetCategoryFirstEntry(p->category) + 1;
    for (i = 0; i < count; i++) {
        Anim_DrawDecimalDigits(p->entryDigits[i], &p->resource, first + i, 2, 0, 0x18,
                               i * 16 + 0x28, 8, 0);
        AnimSprite_Show(&p->rowMarkers[i]);
        p->rowMarkers[i].position.x = 0x28;
        p->rowMarkers[i].position.y = i * 16 + 0x28;
    }
}

void MonsterBook_DrawDetailNumber(struct MonsterBook *p)
{
    MonsterBook_HideRows(p);
    Anim_DrawDecimalDigits(p->entryDigits[0], &p->resource, p->entry + 1, 2, 0, 0x28, 0x18, 8, 0);
}

void MonsterBook_HideCompletionCounts(struct MonsterBook *p)
{
    s32 j;

    for (j = 0; j < 2; j++) {
        p->unlockedDigits[j].flags |= 1;
        p->totalDigits[j].flags |= 1;
    }
    AnimSprite_Hide(&p->countSeparator);
}

void MonsterBook_UpdateSelectionCursor(struct MonsterBook *book)
{
    book->selectionCursor.position.y = book->selectedRow << 4;
}

void MonsterBook_DrawCompletionCounts(struct MonsterBook *p)
{
    s32 group;

    MonsterBook_HideGroupDigits(p);
    group = Bestiary_GetEntryGroup(p->entry);
    Anim_DrawDecimalDigits(p->unlockedDigits, &p->resource,
                           Bestiary_CountUnlockedGroupEntries(group), 2, 0, 0x98, 0x16, 8, 0);
    AnimSprite_Show(&p->countSeparator);
    p->countSeparator.position.x = 0xA8;
    p->countSeparator.position.y = 0x16;
    Anim_DrawDecimalDigits(p->totalDigits, &p->resource, Bestiary_GetGroupEntryCount(group), 2, 0,
                           0xB0, 0x16, 8, 0);
}

void MonsterBook_DrawListWindows(struct MonsterBook *p)
{
    s32 i;
    s32 first, count;

    MonsterBook_ClearWindows(p);
    first = Bestiary_GetCategoryFirstEntry(p->category);
    count = Bestiary_GetCategoryEntryCount(p->category);
    for (i = 0; i < count; i++) {
        s32 e;
        TextWindow_SetScript(p->windows[i], p->script);
        TextWindow_SetBounds(p->windows[i], 6, i * 2 + 5, 0x14, 2);
        e = first + i;
        if (Bestiary_IsEntryUnlocked(e))
            TextWindow_Refresh(p->windows[i], e);
        else
            TextWindow_Refresh(p->windows[i], 0x53);
        TextWindow_StartPlayback(p->windows[i]);
    }
}

void MonsterBook_HideCategoryWidgets(struct MonsterBook *p)
{
    AnimSprite_Hide(&p->categoryBadge);
    AnimSprite_Hide(&p->categoryNumber);
}

void MonsterBook_DrawCategoryWidgets(struct MonsterBook *p)
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
    Anim_LoadKeyframe(&p->categoryBadge, &p->resource, (u16)(frame + 0x41), 2);
    AnimSprite_Show(&p->categoryBadge);
    if (value >= 0) {
        Anim_LoadKeyframe(&p->categoryNumber, &p->resource, (u16)(value + 1), 2);
        AnimSprite_Show(&p->categoryNumber);
        p->categoryNumber.position.x = x;
        p->categoryNumber.position.y = y;
    }
}
