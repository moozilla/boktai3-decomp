#include "global.h"
#include "ui/books/collection_book.h"
#include "ui/books/collection.h"

void ForgingBook_SetupPageBackground(struct CollectionBook *book)
{
    u32 size = 16;
    s32 max, count, row, column;

    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    book->backgroundMap = ScriptProc_Start(0xc091, 0x3536);
    size = 16;
    Bg_SetupLayers(2, 0, book->backgroundMap, 0, 0, 1, &size);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(((struct CollectionPalette *)ScriptProc_Start(0x92b3, 0x204))->colors, gBgPaletteBuffer,
           0x100);
    count = 0;
    max = CollectionBook_GetPageEntryCount(book->page);
    for (row = 0; row < 2; row++) {
        for (column = 0; column < 5; column++) {
            struct BackgroundLayer *layer = &gBgRecords[3];

            layer->width = 16;
            layer->x = 0;
            layer->y = 0;
            count++;
            if (count > max)
                Bg_FillTilemapRectangle(3, column * 5 + 3, row * 5 + 3, 4, 4, 1, 0, 2);
        }
    }
    Bg_FillTilemapRectangle(3, 0, 0, 2, 2, 2, 0, 0);
    Bg_FillTilemapRectangle(3, 28, 0, 2, 2, 2, 0, 0);
}
