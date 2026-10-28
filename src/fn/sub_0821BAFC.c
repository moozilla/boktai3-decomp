#include "global.h"
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void ResourceRegistry_RegisterResource(u32, void *, u32);
void Terrain_ResetMap(void);
extern void *gTerrainMap;
void Terrain_Init(void)
{
    void *p = Heap_Alloc(0xE24);
    Memory_Clear(p, 0xE24);
    ResourceRegistry_RegisterResource(0x56C2, p, 1);
    gTerrainMap = p;
    Terrain_ResetMap();
}
