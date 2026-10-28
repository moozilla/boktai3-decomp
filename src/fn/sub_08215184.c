#include "global.h"
#include "render.h"


struct SpriteTileEntry *AnimData_FindEntry(u16 id)
{
    struct SpriteTileResource *h = gSpriteTileResource;
    if (h == 0)
        return 0;
    {
        struct SpriteTileEntry *e = h->entries;
        s32 i = 0;
        for (; i < h->entryCount; i++, e++) {
            if (e->id == id)
                return e;
        }
    }
    return 0;
}
