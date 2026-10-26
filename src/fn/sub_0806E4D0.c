#include "scene_grid.h"
s32 Div(s32, s32);
void SceneGrid_ComputeCellDelta(s32 first, s32 second, s32 *columnDelta, s32 *rowDelta)
{
    s32 a = first;
    s32 b = second;
    s32 *columns = columnDelta;
    s32 *rows = rowDelta;
    s32 firstRow;
    struct EngineMap **map = &gTerrainMap;
    s32 secondRow;
    s32 width;
    firstRow = Div(a, *(s16 *)((*map)->cells + 4));
    secondRow = Div(b, *(s16 *)((*map)->cells + 4));
    width = *(s16 *)((*map)->cells + 4);
    a -= firstRow * width;
    b -= secondRow * width;
    b -= a;
    *columns = b;
    {
        s32 quotient = firstRow;
        secondRow -= quotient;
    }
    {
        s32 *out = rows;
        *out = secondRow;
    }
}
