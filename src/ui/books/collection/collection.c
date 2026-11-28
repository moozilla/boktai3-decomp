#include "ui/books/collection.h"
#include "system/game_state/game_state.h"
#include "engine/video/render.h"

u32 Collection_GetPageEntryCount(u32 i)
{
    return gUnk_08613FB4[i];
}

/* Collection_SetEntryUnlocked: Sets the indexed bit in the persistent collection unlock mask */

void Collection_SetEntryUnlocked(s32 bit)
{
    gGameState->flags728 |= 1 << bit;
}

u32 Collection_IsEntryUnlocked(u32 i)
{
    return gGameState->flags728 & (1 << i);
}

void Collection_UnlockEntryFromScript(void)
{
    if (Script_SeekToKeyword(0x69)) {
        u32 entry = Script_GetValue();

        gGameState->flags728 |= 1 << entry;
    }
}

s32 Collection_CountUnlockedEntries()
{
    s32 count = 0;
    s32 i = 0;
    s32 unlocked = gGameState->flags728;

    for (; i < 27; i++) {
        if ((unlocked >> i) & 1)
            count++;
    }
    return count;
}

u32 Collection_IsComplete(void)
{
    return Collection_CountUnlockedEntries() > 26;
}

/* Tentative: bit 0 hides an entry display outside the unlocked current page. */

s32 Collection_UpdateEntryVisibility(struct CollectionMenu *menu)
{
    s32 i;

    for (i = 0; i < 27; i++)
        menu->entries[i].flags |= 1;
    for (i = menu->page * 10; i < menu->page * 10 + 10; i++) {
        if (Collection_IsEntryUnlocked(i))
            menu->entries[i].flags &= ~1;
        if (i > 25)
            break;
    }
    return 0;
}

s32 Collection_UpdateCountDigits(struct CollectionMenu *p)
{
    s32 unlocked = Collection_CountUnlockedEntries(p), unlockedTens = Div(unlocked, 10),
        unlockedOnes = Mod(unlocked, 10), totalTens = Div(27, 10), totalOnes = Mod(27, 10);
    Anim_LoadKeyframe(&p->countDigits[0], &p->resource, (u16)unlockedTens, 0);
    Anim_LoadKeyframe(&p->countDigits[1], &p->resource, (u16)unlockedOnes, 0);
    Anim_LoadKeyframe(&p->countDigits[2], &p->resource, (u16)totalTens, 0);
    Anim_LoadKeyframe(&p->countDigits[3], &p->resource, (u16)totalOnes, 0);
    return 0;
}

u32 Collection_UpdatePageIndicator(struct CollectionMenu *p)
{
    Anim_LoadKeyframe(&p->pageIndicator, &p->resource, (u16)(p->page + 0x59), 0);
    return 0;
}

s32 Collection_UpdateCursor(struct CollectionMenu *p)
{
    p->cursor.position.x = p->column * 40;
    p->cursor.position.y = p->row * 40;
    Anim_LoadKeyframe(&p->cursor, &p->resource, 0x58, 0);
    return 0;
}

void Collection_UpdateHighlightPulse(struct CollectionMenu *p)
{
    s16 frame = p->pulseFrame;
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
    p->pulseFrame++;
    if (p->pulseFrame > 0x31)
        p->pulseFrame = 0;
    p->cursorColors[12] = color;
}

u32 Collection_UpdateSelectedEntryText(struct CollectionMenu *p)
{
    s32 id;

    if (Collection_IsEntryUnlocked((s16)p->selectedEntry)) {
        TextWindow_SetScript(p->textWindow, p->nameScript);
        TextWindow_Refresh(p->textWindow, (s16)p->selectedEntry);
        TextWindow_StartPlayback(p->textWindow);
        ScrollingDialogue_ClearScript();
        id = Script_ParseStringRef(p->descriptionScript);
        ScrollingDialogue_SetScript(Text_LookupString(id + (s16)p->selectedEntry));
    } else {
        TextWindow_SetScript(p->textWindow, p->nameScript);
        TextWindow_Refresh(p->textWindow, 27);
        TextWindow_StartPlayback(p->textWindow);
        ScrollingDialogue_ClearScript();
        id = Script_ParseStringRef(p->lockedDescriptionScript);
        ScrollingDialogue_SetScript(Text_LookupString(id + (s16)p->selectedEntry));
    }
    return 0;
}

void Collection_LoadPageBackground(struct CollectionMenu *p)
{
    u32 size;
    s32 max, count, row;
    struct BackgroundLayer *tile;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    {
        u8 *res = ScriptProc_Start(0xc091, 0x3536);
        p->backgroundMap = res;
        size = 16;
        Bg_SetupLayers(2, 0, res, 0, 0, 1, &size);
    }
    Bg_SetPriority(3, 3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(((struct CollectionPalette *)ScriptProc_Start(0x92b3, 0x204))->colors, gBgPaletteBuffer,
           0x100);
    count = 0;
    max = Collection_GetPageEntryCount(p->page);
    row = 0;
    tile = &gBgRecords[3];
    do {
        int next = row + 1;
        int x = 3, y = row * 5, i = 4;
        do {
            tile->width = 16;
            tile->x = 0;
            tile->y = 0;
            count++;
            if (count > max)
                Bg_FillTilemapRectangle(3, x, y + 3, 4, 4, 1, 0, 2);
            x += 5;
            i--;
        } while (i >= 0);
        row = next;
    } while (row <= 1);
}

s32 Collection_RefreshPage(struct CollectionMenu *s)
{
    Collection_LoadPageBackground(s);
    Collection_UpdateEntryVisibility(s);
    Collection_UpdateCursor(s);
    Collection_UpdateCountDigits(s);
    Collection_UpdatePageIndicator(s);
    Collection_UpdateSelectedEntryText(s);
    return 0;
}

u32 Collection_RefreshSelection(struct CollectionMenu *a)
{
    Collection_UpdateCursor(a);
    Collection_UpdateCountDigits(a);
    Collection_UpdateSelectedEntryText(a);
    return 0;
}
