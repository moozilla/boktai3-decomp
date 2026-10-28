#include "global.h"
#include "text.h"

struct FontResource *Heap_Alloc(u32);
void Memory_Clear(struct FontResource *, u32);
struct FontResource *ScriptProc_Start(u32, u32);

s32 Text_LoadFontResource(void)
{
    if (gFontResource != 0)
        return 0;
    gFontResource = Heap_Alloc(12);
    if (gFontResource != 0) {
        struct FontResource *p;
        Memory_Clear(gFontResource, 12);
        p = ScriptProc_Start(0xA635, 0x3F51);
        if (p != 0) {
            *gFontResource = *p;
            gFontResource->narrowGlyphData += (u32)p;
            gFontResource->wideGlyphData += (u32)p;
            return 0;
        }
    }
    return -1;
}
