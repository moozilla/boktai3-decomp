#include "scene_tilemap.h"

void SaveMenu_ClearSingleton(void)
{
    gSaveMenu = 0;
}

void SaveMenu_Close(void)
{
    if (gSaveMenu) {
        LinkScorePanel_HideSprites();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        ScrollingDialogue_ClearScript();
        Task_Kill((void *)gSaveMenu);
    }
}

void SceneEffect_ExecuteCommandList(const s16 *commands, s32 length)
{
    s32 i;
    for (i = 0; i < length; i += 3) {
        switch (commands[i + 2]) {
            case 0:
                sub_08115AC8(commands[i], commands[i + 1], 0, 30);
                break;
            case 4:
                sub_08115AC8(commands[i], commands[i + 1], 2, 60);
                break;
            case 1:
                sub_08115AC8(commands[i], commands[i + 1], 1, 80);
                break;
            case 2:
                sub_081160CC(commands[i], commands[i + 1], 0);
                break;
            case 3:
                sub_081160CC(commands[i], commands[i + 1], 1);
                break;
        }
    }
}

u8 SceneTilemap_IsActive(void)
{
    if (gSceneCamera == NULL) {
        return 0;
    }
    return 1;
}

void TileRows_CopyPackedRows(u16 *dest, s32 page, s32 offset, s32 firstRow, s32 width, s32 height,
                             const u16 *source)
{
    s32 row;
    u32 count;
    u16 *dst;
    const u16 *src;
    for (row = 0; row < height; row++) {
        if (width > 0) {
            s32 index = (row + firstRow) * 4 + (offset + page * 80);
            count = row * 64 + (u32)dest;
            dst = (u16 *)count;
            count = width;
            src = (const u16 *)(index * 2 + (u32)source);
            do {
                *dst++ = *src++;
            } while (--count != 0);
        }
    }
}

void TilemapPanel_DrawRegion(struct TilemapPanel *panel, s32 col, s32 row, s32 x0, s32 y0, s32 x1,
                             s32 y1, s32 cellWidth, s32 cellHeight, s32 offset, s32 firstRow)
{
    s32 base = col + row * 128;
    s32 index = base;
    u16 *screen = panel->screenMap;
    s32 x, y;
    for (y = y0; y < y1; y++) {
        s32 rowOff = panel->scroll.width * y;
        for (x = x0; x < x1; x++) {
            TileRows_CopyPackedRows(&screen[index], panel->map[x + rowOff], offset, firstRow,
                                    cellWidth, cellHeight, panel->tileData);
            index += cellWidth;
        }
        index = base + (y + 1) * 128;
    }
}

void SceneTilemap_WaitCopyDelay(void)
{
    s32 row = 0;
    do {
        s32 next = row + 1;
        s32 column = 31;
        do {
            column--;
        } while (column >= 0);
        row = next;
    } while (row <= 0);
}

void TilemapPanel_StreamColumns(struct TilemapPanel *panel)
{
    struct TilemapScroll *scroll = &panel->scroll;
    s32 stripOffset;
    s32 firstRow;
    s32 scrollX;
    s32 visibleRight;
    s32 lastTile;
    s32 distance;
    s32 rightEdge;
    s32 tileX;
    s32 cursorX;
    s32 column;
    s32 cellWidth;
    scrollX = Mod(panel->scroll.x, 256);
    Mod(scroll->y, 256);
    gBgRecords[2].x = scrollX;
    lastTile = scroll->width - 12;
    visibleRight = panel->scroll.x + 240;
    stripOffset = panel->drawnX;
    distance = stripOffset - visibleRight;
    if (distance < 8) {
        tileX = panel->scroll.x >> 5;
        rightEdge = panel->drawnX + 7;
        if (tileX > lastTile) {
            rightEdge = panel->drawnX;
            panel->atEnd = 1;
        }
    } else
        rightEdge = panel->drawnX;
    if (rightEdge == panel->drawnX)
        return;
    cursorX = rightEdge >> 5;
    column = (rightEdge & 0xff) >> 3;
    if (distance < 0) {
        cellWidth = 2;
        rightEdge += 9;
    } else
        cellWidth = 1;
    switch (column & 3) {
        case 0:
            stripOffset = 0;
            firstRow = 0;
            TilemapPanel_DrawRegion(panel, column, 0, cursorX, firstRow, cursorX + 1, 1, cellWidth,
                                    20, stripOffset, firstRow);
            cursorX = rightEdge;
            break;

        case 1:
            stripOffset = 1;
            firstRow = 0;
            TilemapPanel_DrawRegion(panel, column, 0, cursorX, firstRow, cursorX + 1, 1, cellWidth,
                                    20, stripOffset, firstRow);
            cursorX = rightEdge;
            break;

        case 2:
            stripOffset = 2;
            firstRow = 0;
            TilemapPanel_DrawRegion(panel, column, firstRow, cursorX, firstRow, cursorX + 1, 1,
                                    cellWidth, 20, stripOffset, firstRow);
            cursorX = rightEdge;
            break;

        case 3:
            stripOffset = 3;
            firstRow = 0;
            if (cellWidth == 1) {
                TilemapPanel_DrawRegion(panel, column, 0, cursorX, firstRow, cursorX + 1, 1,
                                        cellWidth, 20, stripOffset, firstRow);
                cursorX = rightEdge;
            } else {
                cellWidth = 1;
                TilemapPanel_DrawRegion(panel, column, 0, cursorX, firstRow, cursorX + 1, 1,
                                        cellWidth, 20, stripOffset, firstRow);
                cursorX++;
                TilemapPanel_DrawRegion(panel, (column + 1) & 31, 0, cursorX, firstRow, cursorX + 1,
                                        1, cellWidth, 20, firstRow, firstRow);
                cursorX = rightEdge;
            }
            break;
        default:
            cursorX = rightEdge;
            break;
    }

    panel->drawnX = (cursorX >> 3) << 3;
}

s32 SceneTilemap_Update(struct TilemapPanel *panel)
{
    panel->scroll.previousX = panel->scroll.x;
    if (panel->scrollSource && panel->atEnd != 1)
        panel->scroll.x = panel->scrollSource->x;
    TilemapPanel_StreamColumns(panel);
    return 0;
}

u32 SceneTilemap_Destroy(struct TilemapPanel *panel)
{
    Heap_Free(panel->map);
    Heap_Free(panel->tiles);
    gSceneCamera = 0;
    return 0;
}

void Bg_SetControlFields(s32 id, u32 screenSize, u32 screenBase, u32 color256, u32 mosaic,
                         u32 characterBase)
{
    screenSize <<= 14;
    screenBase <<= 8;
    color256 <<= 7;
    mosaic <<= 6;
    characterBase <<= 2;
    switch (id) {
        case 0:
            *(vu16 *)0x04000008 = characterBase | (mosaic | (screenSize | screenBase | color256));
            break;
        case 1:
            *(vu16 *)0x0400000A = screenSize | screenBase | color256 | mosaic | characterBase | 1;
            break;
        case 2:
            *(vu16 *)0x0400000C = screenSize | screenBase | color256 | mosaic | characterBase | 2;
            break;
        case 3:
            *(vu16 *)0x0400000E = screenSize | screenBase | color256 | mosaic | characterBase | 3;
            break;
    }
}

void SceneTilemap_LoadLayoutFromScript(struct TilemapPanel *panel)
{
    u8 *values;
    s32 length, index, value, padding, allocationSize;
    if (Script_SeekToKeyword('s')) {
        values = Heap_Alloc(0x1000);
        panel->mapScript = (u8 *)Script_GetValueSafe();
        Script_SetPc(panel->mapScript);
        for (panel->mapSize = 0; (value = Script_GetValue()) >= 0; panel->mapSize++)
            values[panel->mapSize] = value;
        length = panel->mapSize;
        padding = 3 & length;
        if (padding > 0) {
            padding = 4 - padding;
            allocationSize = length + padding;
        } else
            allocationSize = length;
        panel->map = Heap_Alloc(allocationSize);
        Memory_Clear(panel->map, allocationSize);
        for (index = 0; index < panel->mapSize; index++)
            panel->map[index] = values[index];
        Heap_Free(values);
    }
}

void TilemapPanel_LoadScriptTileList(struct TilemapPanel *panel)
{
    s32 *values;
    if (Script_SeekToKeyword('g')) {
        values = Heap_Alloc(0x400);
        panel->tileListScript = (u8 *)Script_GetValueSafe();
        Script_SetPc(panel->tileListScript);
        panel->tileCount = 0;
        for (;;) {
            values[panel->tileCount] = Script_GetValue();
            if (values[panel->tileCount] < 0)
                break;
            panel->tileCount++;
        }
        panel->tiles = Heap_Alloc(panel->tileCount * 2);
        Memory_Clear(panel->tiles, panel->tileCount * 2);
        {
            s32 index;
            for (index = 0; index < panel->tileCount; index++)
                panel->tiles[index] = values[index];
        }
        Heap_Free(values);
    }
}

s32 TilemapPanel_Initialize(struct TilemapPanel *panel)
{
    s32 choice;
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                             sub_08219624);
    SceneTilemap_LoadLayoutFromScript(panel);
    TilemapPanel_LoadScriptTileList(panel);
    panel->screenMap = Video_GetBackgroundMap(2);
    panel->scroll.width = panel->mapSize;
    panel->scroll.height = 1;
    Terrain_SetColumnLayout(panel->map);
    Bg_SetControlFields(2, 0, 29, 0, 0, 0);
    choice = Script_GetKeywordValue('c', 0);
    switch (choice) {
        case 0:
            Bg_SetTileUpload(2, gUnk_0860B2D0, 0, 0x188);
            Terrain_SetColumnDefinitions(gUnk_08607DB0);
            panel->tileData = (const u16 *)gUnk_08606550;
            break;
        case 1:
            Bg_SetTileUpload(2, gUnk_0860E3D0, 0, 0x117);
            Terrain_SetColumnDefinitions(gUnk_0860A470);
            panel->tileData = (const u16 *)gUnk_08609610;
            break;
    }
    Bg_SetPriority(2, 3);
    *(vu16 *)0x04000018 = 0;
    {
        u32 mask = 0x400;
        gDispCntShadow = mask | gDispCntShadow;
    }
    gBgRecords[2].y = 0;
    TilemapPanel_DrawRegion(panel, 0, 0, 0, 0, 8, 1, 4, 20, 0, 0);
    panel->drawnX = 0x100;
    gSceneCamera = (struct Camera *)panel;
    SceneEffect_ExecuteCommandList((const s16 *)panel->tiles, panel->tileCount);
    panel->value38 = 0xa000;
    return 0;
}

struct TilemapPanel *SceneTilemap_CreateFromScript(void)
{
    struct TilemapPanel *panel;
    if (gSceneCamera != 0)
        return (struct TilemapPanel *)gSceneCamera;
    panel = Task_Create(2, sizeof(struct TilemapPanel));
    if (panel != 0) {
        Task_SetCallbacks(panel, (void (*)(void *))SceneTilemap_Update,
                          (void (*)(void *))SceneTilemap_Destroy);
        if (TilemapPanel_Initialize(panel) < 0) {
            Task_Kill(panel);
            return 0;
        }
    }
    return panel;
}

s32 SceneTilemap_GetLayoutLength(void)
{
    if (!((struct TilemapPanel *)gSceneCamera))
        return -1;
    else
        return ((struct TilemapPanel *)gSceneCamera)->mapSize;
}

s32 SceneTilemap_GetScrollDelta(void)
{
    struct TilemapPanel *panel = (struct TilemapPanel *)gSceneCamera;
    if (!panel)
        return 0;
    else
        return panel->scroll.x - panel->scroll.previousX;
}

void SceneTilemap_SetScrollSource(struct TilemapScrollSource *source)
{
    if (SceneTilemap_IsActive())
        ((struct TilemapPanel *)gSceneCamera)->scrollSource = source;
}

void SceneTilemap_CopyScrollSourceState(struct TilemapScrollSource *dst)
{
    if (SceneTilemap_IsActive()) {
        *dst = *((struct TilemapPanel *)gSceneCamera)->scrollSource;
    }
}

s32 SceneTilemap_GetScrollLimit(void)
{
    if (!SceneTilemap_IsActive())
        return -1;
    else
        return ((struct TilemapPanel *)gSceneCamera)->scroll.width - 12;
}
