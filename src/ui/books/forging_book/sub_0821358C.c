#include "global.h"
#include "ui/books/collection_book.h"
extern const u32 gUnk_08614054[];
extern const u32 gUnk_0861407C[];
extern const u32 gUnk_086140A4[];
void AnimSprite_InitLinked(void *, void *, u32, u32, u32, u32, u32, const void *);
s32 Mod(s32, s32);

void sub_0821358C(struct CollectionBook *book)
{
    struct AnimSpriteResource *resource;
    struct ContactPosition position;
    s32 i;

    resource = ScriptProc_Start(0xcb05, 0xe2ab);
    book->resource = *resource;
    ResourceHeader_Relocate(&book->resource, resource);
    AnimSprite_InitLinked(&book->background, &book->resource, 0x57, 0x10, 0, 0, 0, 0);
    AnimSprite_InitLinked(&book->cursor, &book->resource, 0x58, 0x10, 0, 0, 0, 0);
    AnimSprite_InitLinked(&book->completionBadge, &book->resource, 0x3e, 0x10, 0, 0, 0, 0);
    if (!CollectionBook_IsComplete())
        book->completionBadge.flags |= 1;
    position.x = 0xb8;
    position.y = 0x68;
    position.z = 0;
    for (i = 0; i < 4; i++) {
        AnimSprite_InitLinked(&book->countDigits[i], &book->resource, 0, 0x10, 0, 0, 0, &position);
        position.x += 8;
        if (i == 1)
            position.x += 8;
    }
    AnimSprite_InitLinked(&book->pageHeading, &book->resource, 0x59, 0x10, 0, 0, 0, 0);
    resource = ScriptProc_Start(0xcb05, 0x530d);
    book->markerResource = *resource;
    ResourceHeader_Relocate(&book->markerResource, resource);
    AnimSprite_InitLinked(&book->trackedMarker, &book->markerResource, 0x50, 0x10, 0, 0, 0, 0);
    position.x = 0x28;
    position.y = 0x28;
    for (i = 0; i < 10; i++) {
        AnimData_Load(&book->graphics[i], 0xca1f);
        Sprite_Init(&book->entries[i], &book->graphics[i], 0x11);
        {
            /* Only the frame store needs the entry address built this way (book->entries[i]
             * would let the position stores reuse it). */
            const u32 *frame = &gUnk_08614054[i];
            struct DisplaySequenceSprite *entry = (void *)(i * sizeof(*entry) + (u32)book);

            entry = (void *)((u8 *)entry + 0x500);
            entry->frame = *frame;
        }
        book->entries[i].position.x = position.x;
        book->entries[i].position.y = position.y;
        book->entries[i].position.z = 0;
        book->entries[i].priority = 1;
        AnimData_SelectPaletteBank((void *)book->entries[i].resource, gUnk_086140A4[i] + 0x17d);
        if (Mod(i, 10) == 4) {
            position.x = 0x28;
            position.y += 0x28;
        } else {
            position.x += 0x28;
        }
        if (Mod(i, 10) == 9) {
            position.x = 0x28;
            position.y = 0x28;
        }
        book->entryIds[i] = gUnk_0861407C[i];
    }
    CpuSet(book->cursor.palette, book->cursorColors, 0x10);
    book->cursor.palette = book->cursorColors;
    book->cursor.paletteBank = 0xe010;
}
