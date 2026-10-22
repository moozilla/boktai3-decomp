#include "global.h"
#define Background_LoadScriptTileList TilemapPanel_LoadScriptTileList
struct Background {
    u8 pad0[0x2c];
    u8 *tileListScript;
    u16 *tiles;
    s32 tileCount;
};
void *Heap_Alloc(u32);
void Heap_Free(void *);
void Memory_Clear(void *, u32);
s32 Script_GetValue(void);
s32 Script_GetValueSafe(void);
s32 Script_SeekToKeyword(u32);
void Script_SetPc(u8 *);
void TilemapPanel_LoadScriptTileList(struct Background *bg)
{
    s32 *values;
    if (Script_SeekToKeyword('g')) {
        values = Heap_Alloc(0x400);
        bg->tileListScript = (u8 *)Script_GetValueSafe();
        Script_SetPc(bg->tileListScript);
        bg->tileCount = 0;
        for (;;) {
            values[bg->tileCount] = Script_GetValue();
            if (values[bg->tileCount] < 0)
                break;
            bg->tileCount++;
        }
        bg->tiles = Heap_Alloc(bg->tileCount * 2);
        Memory_Clear(bg->tiles, bg->tileCount * 2);
        {
            s32 i;
            for (i = 0; i < bg->tileCount; i++)
                bg->tiles[i] = values[i];
        }
        Heap_Free(values);
    }
}
