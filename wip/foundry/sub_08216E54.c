#define BgGfx_LoadMapResource   sub_08216E54
#define gCachedMapKey           gUnk_03001548
#define gMapDecompressionBuffer gUnk_02020400
#include "global.h"
void *ScriptProc_Start(u16, u16);
void LZ77UnCompWram(const u32 *, void *);
extern u16 gUnk_03001548;
extern u8 gUnk_02020400[], gUnk_02020404[];
void *sub_08216E54(u16 key)
{
    u8 *resource = ScriptProc_Start(0xc091, key);
    if (!resource)
        return 0;
    if ((resource[0] == 0x4d && resource[1] == 0x50) || *(u32 *)resource == 0x005e8cc5)
        return resource;
    if (gUnk_03001548 != key) {
        gUnk_03001548 = key;
        LZ77UnCompWram((u32 *)resource, gUnk_02020400);
        return gUnk_02020404;
    }
    return gUnk_02020404;
}
