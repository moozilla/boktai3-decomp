#include "global.h"
#include "ui/town/storage_screen.h"
void Storage_Destroy(struct StorageScreen *screen)
{
    struct AnimSprite *sprites = screen->controls;
    s32 i;
    for (i = 0; i < 56; i++) {
        if (sprites[i].active)
            Anim_Remove(&sprites[i]);
    }
    SceneInput_DestroyMarkerPair(&screen->markers);
    SceneInput_DestroyGlyphPair(&screen->glyphs);
}
