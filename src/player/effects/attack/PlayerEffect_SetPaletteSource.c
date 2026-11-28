#include "global.h"
#include "engine/video/render.h"
#include "engine/task.h"
#include "scene/scene_animation.h"

struct PaletteSourceEffect {
    struct Task task;
    struct SceneAnimation graphics;
};

void PlayerEffect_SetPaletteSource(struct PaletteSourceEffect *effect, u32 paletteIndex,
                                   u32 palette)
{
    if (effect != NULL) {
        effect->graphics.paletteIndex = paletteIndex;
        if (palette == 0)
            effect->graphics.palette = gPaletteBankData + 0x540;
        else
            effect->graphics.palette = (void *)palette;
    }
}
