#include "global.h"

#include "render.h"

s32 Mod(s32, s32);

static inline s32 ScreenBlock(s32 tile)
{
    return tile >= 0 ? tile >> 5 : -((-tile) >> 5);
}

/* Expand the left or right column of a 2x2 metatile into two tilemap rows. */
#define WRITE_METATILE_COLUMN(dst, definitions, id, column, upperRow, lowerRow)                    \
    {                                                                                              \
        u16 metatile = (id);                                                                       \
        if (!((column) & 1)) {                                                                     \
            (dst)[(column) + (upperRow)] = (definitions)[(metatile << 2) + 0];                     \
            (dst)[(column) + (lowerRow)] = (definitions)[(metatile << 2) + 2];                     \
        } else {                                                                                   \
            (dst)[(column) + (upperRow)] = (definitions)[(metatile << 2) + 1];                     \
            (dst)[(column) + (lowerRow)] = (definitions)[(metatile << 2) + 3];                     \
        }                                                                                          \
    }

void BgGfx_LoadTilemap(u32 layer, s32 tileX, s32 tileY, s32 screenX, s32 screenY, u16 *map)
{
    const s16 lastTile = 31;
    struct BackgroundLayer *record = &gBgRecords[layer];
    s32 deltaX, deltaY, width, height, right, bottom, pitch, column, rowOffset;
    u16 *window;
    const u16 *tiles;
    u16 fill;
    s32 block, columnX;
    deltaX = tileX - (s16)record->scaleX;
    deltaY = tileY - (s16)record->scaleY;
    record->scaleX = tileX;
    record->scaleY = tileY;
    record->x = screenX;
    record->y = screenY;
    if (deltaX == 0 && deltaY == 0)
        return;
    window = record->map;
    if (map == NULL)
        map = record->mapData;
    tiles = record->extraData;
    width = record->halfWidth * 2;
    height = record->halfHeight * 2;
    right = width - 32;
    bottom = height - 32;
    pitch = record->width >> 4;
    if ((s16)record->flags != 1) {
        /* A window crossing a map edge needs explicit fill cells. */
        if (tileX < 0 || tileY < 0 || tileX >= right || tileY >= bottom) {
            if (record->flags == 2)
                fill = tiles[map[0]];
            else
                fill = 0;
            if (deltaX < -1 || deltaX > 1 || deltaY > 2 || deltaY < -2) {
                for (rowOffset = 0; rowOffset < 32; rowOffset += 2) {
                    s32 row, nextRow, blockX, blockRow, metatileRow, x, y;
                    y = tileY + rowOffset;
                    if (y < 0 || y >= height) {
                        y &= 31;
                        row = y << 5;
                        nextRow = (y + 1) << 5;
                        for (column = 0; column < 32; column++) {
                            x = (tileX + column) & 31;
                            window[x + row] = fill;
                            window[x + nextRow] = fill;
                        }
                    } else {
                        blockRow = ScreenBlock(y);
                        blockRow *= pitch;
                        y &= 31;
                        metatileRow = (y >> 1) << 4;
                        row = y << 5;
                        nextRow = (y + 1) << 5;
                        for (column = 0; column < 32; column++) {
                            x = tileX + column;
                            if (x < 0 || x >= width) {
                                x &= 31;
                                window[x + row] = fill;
                                window[x + nextRow] = fill;
                            } else {
                                blockX = x >> 5;
                                block = (blockX + blockRow) << 8;
                                x &= 31;
                                WRITE_METATILE_COLUMN(window, tiles,
                                                      map[block + (x >> 1) + metatileRow], x, row,
                                                      nextRow);
                            }
                        }
                    }
                }
            } else {
                if (deltaX) {
                    column = 0;
                    if (deltaX > 0)
                        column = 31;
                    {
                        s32 blockX, blockRow, metatileX, y;
                        columnX = tileX + column;
                        if (columnX < 0 || columnX >= width) {
                            columnX &= 31;
                            for (rowOffset = 0; rowOffset < 32; rowOffset += 2) {
                                y = (tileY + rowOffset) & 31;
                                window[columnX + (y << 5)] = fill;
                                window[columnX + ((y + 1) << 5)] = fill;
                            }
                        } else {
                            blockX = ScreenBlock(columnX);
                            columnX &= 31;
                            metatileX = columnX >> 1;
                            for (rowOffset = 0; rowOffset < 32; rowOffset += 2) {
                                y = tileY + rowOffset;
                                if (y < 0 || y >= height) {
                                    y &= 31;
                                    window[columnX + (y << 5)] = fill;
                                    window[columnX + ((y + 1) << 5)] = fill;
                                } else {
                                    blockRow = y >> 5;
                                    block = (blockX + blockRow * pitch) << 8;
                                    y &= 31;
                                    WRITE_METATILE_COLUMN(window, tiles,
                                                          map[block + metatileX + ((y >> 1) << 4)],
                                                          columnX, (y << 5), ((y + 1) << 5));
                                }
                            }
                        }
                    }
                }
                if (deltaY) {
                    rowOffset = 0;
                    if (deltaY > 0)
                        rowOffset = 30;
                    {
                        s32 row, nextRow, blockX, blockRow, metatileRow;
                        /* The scroll value was saved to the record above. Reuse it for the exposed row. */
                        screenX = tileY + rowOffset;
                        if (screenX < 0 || screenX >= height) {
                            screenX &= 31;
                            row = screenX << 5;
                            nextRow = (screenX + 1) << 5;
                            for (column = 0; column < 32; column++) {
                                columnX = (tileX + column) & 31;
                                window[columnX + row] = fill;
                                window[columnX + nextRow] = fill;
                            }
                        } else {
                            blockRow = ScreenBlock(screenX);
                            blockRow *= pitch;
                            screenX &= 31;
                            metatileRow = (screenX >> 1) << 4;
                            row = screenX << 5;
                            nextRow = (screenX + 1) << 5;
                            for (column = 0; column < 32; column++) {
                                columnX = tileX + column;
                                if (columnX < 0 || columnX >= width) {
                                    columnX &= lastTile;
                                    window[columnX + row] = fill;
                                    window[columnX + nextRow] = fill;
                                } else {
                                    blockX = columnX >> 5;
                                    block = (blockX + blockRow) << 8;
                                    columnX &= lastTile;
                                    WRITE_METATILE_COLUMN(window, tiles,
                                                          map[block + (columnX >> 1) + metatileRow],
                                                          columnX, row, nextRow);
                                }
                            }
                        }
                    }
                }
            }
        } else {
            if (deltaX < -1 || deltaX > 1 || deltaY > 2 || deltaY < -2) {
                for (rowOffset = 0; rowOffset < 32; rowOffset += 2) {
                    s32 row, nextRow, blockX, blockRow, metatileRow, x, y;
                    y = tileY + rowOffset;
                    blockRow = ScreenBlock(y);
                    blockRow *= pitch;
                    y &= 31;
                    metatileRow = (y >> 1) << 4;
                    row = y << 5;
                    nextRow = (y + 1) << 5;
                    for (column = 0; column < 32; column++) {
                        x = tileX + column;
                        blockX = ScreenBlock(x);
                        block = (blockX + blockRow) << 8;
                        x &= 31;
                        WRITE_METATILE_COLUMN(window, tiles, map[block + (x >> 1) + metatileRow], x,
                                              row, nextRow);
                    }
                }
            } else {
                if (deltaY) {
                    s32 start, limit;
                    if (deltaY > 0) {
                        start = 32 - deltaY;
                        limit = 32;
                    } else {
                        start = 0;
                        limit = -deltaY;
                    }
                    for (rowOffset = start; rowOffset < limit; rowOffset += 2) {
                        s32 row, nextRow, blockX, blockRow, metatileRow, x, y;
                        y = tileY + rowOffset;
                        blockRow = ScreenBlock(y);
                        blockRow *= pitch;
                        y &= 31;
                        metatileRow = (y >> 1) << 4;
                        row = y << 5;
                        nextRow = (y + 1) << 5;
                        for (column = 0; column < 32; column++) {
                            x = tileX + column;
                            blockX = ScreenBlock(x);
                            block = (blockX + blockRow) << 8;
                            x &= 31;
                            WRITE_METATILE_COLUMN(window, tiles,
                                                  map[block + (x >> 1) + metatileRow], x, row,
                                                  nextRow);
                        }
                    }
                }
                if (deltaX) {
                    s32 start, limit;
                    if (deltaX > 0) {
                        start = 32 - deltaX;
                        limit = 32;
                    } else {
                        start = 0;
                        limit = -deltaX;
                    }
                    for (column = start; column < limit; column++) {
                        s32 blockX, blockRow, metatileX, x, y;
                        x = tileX + column;
                        blockX = ScreenBlock(x);
                        x &= 31;
                        metatileX = x >> 1;
                        for (rowOffset = 0; rowOffset < 32; rowOffset += 2) {
                            y = tileY + rowOffset;
                            blockRow = ScreenBlock(y);
                            block = (blockX + blockRow * pitch) << 8;
                            y &= 31;
                            WRITE_METATILE_COLUMN(window, tiles,
                                                  map[block + metatileX + ((y >> 1) << 4)], x,
                                                  (y << 5), ((y + 1) << 5));
                        }
                    }
                }
            }
        }
    } else {
        tileX = sub_08249350(tileX, width);
        tileY = sub_08249350(tileY, height);
        if (tileX < 0)
            tileX += width;
        if (tileY < 0)
            tileY += height;
        if (deltaX < -1 || deltaX > 1 || deltaY > 2 || deltaY < -2) {
            for (rowOffset = 0; rowOffset < 32; rowOffset += 2) {
                s32 row, nextRow, blockX, blockRow, metatileRow, x, y;
                y = tileY + rowOffset;
                blockRow = ScreenBlock(Mod(y, height));
                blockRow *= pitch;
                y &= 31;
                metatileRow = (y >> 1) << 4;
                row = y << 5;
                nextRow = (y + 1) << 5;
                for (column = 0; column < 32; column++) {
                    x = tileX + column;
                    blockX = ScreenBlock(Mod(x, width));
                    block = (blockX + blockRow) << 8;
                    x &= 31;
                    WRITE_METATILE_COLUMN(window, tiles, map[block + (x >> 1) + metatileRow], x,
                                          row, nextRow);
                }
            }
        } else {
            if (deltaY) {
                s32 start, limit;
                if (deltaY > 0) {
                    start = 32 - deltaY;
                    limit = 32;
                } else {
                    start = 0;
                    limit = -deltaY;
                }
                for (rowOffset = start; rowOffset < limit; rowOffset += 2) {
                    s32 row, nextRow, blockX, blockRow, metatileRow, x, y;
                    y = tileY + rowOffset;
                    blockRow = ScreenBlock(Mod(y, height));
                    blockRow *= pitch;
                    y &= 31;
                    metatileRow = (y >> 1) << 4;
                    row = y << 5;
                    nextRow = (y + 1) << 5;
                    for (column = 0; column < 32; column++) {
                        x = tileX + column;
                        blockX = ScreenBlock(Mod(x, width));
                        block = (blockX + blockRow) << 8;
                        x &= 31;
                        WRITE_METATILE_COLUMN(window, tiles, map[block + (x >> 1) + metatileRow], x,
                                              row, nextRow);
                    }
                }
            }
            if (deltaX) {
                s32 start, limit;
                if (deltaX > 0) {
                    start = 32 - deltaX;
                    limit = 32;
                } else {
                    start = 0;
                    limit = -deltaX;
                }
                for (column = start; column < limit; column++) {
                    s32 blockX, blockRow, metatileX, x, y;
                    x = tileX + column;
                    blockX = ScreenBlock(Mod(x, width));
                    x &= 31;
                    metatileX = x >> 1;
                    for (rowOffset = 0; rowOffset < 32; rowOffset += 2) {
                        y = tileY + rowOffset;
                        blockRow = ScreenBlock(Mod(y, height));
                        block = (blockX + blockRow * pitch) << 8;
                        y &= 31;
                        WRITE_METATILE_COLUMN(window, tiles,
                                              map[block + metatileX + ((y >> 1) << 4)], x, (y << 5),
                                              ((y + 1) << 5));
                    }
                }
            }
        }
    }
}