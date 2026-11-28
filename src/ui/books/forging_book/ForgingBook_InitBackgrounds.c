#include "global.h"
#include "ui/books/forging_book.h"
#include "ui/books/collection.h"

void ForgingBook_InitBackgrounds(struct ForgingBook *book)
{
    u32 size = 16;

    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    book->backgroundMap = ScriptProc_Start(0xc091, 0x3536);
    size = 15;
    Bg_SetupLayers(2, 0, book->backgroundMap, 0, 0, 1, &size);
    Bg_SetPriority(3, 2);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(((struct CollectionPalette *)ScriptProc_Start(0x92b3, 0x204))->colors, gBgPaletteBuffer,
           0x100);
}
