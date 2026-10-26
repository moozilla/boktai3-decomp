#include "global.h"
#include "render.h"

struct SceneScriptHitSource {
    u8 unknown0[0x38];
    u32 flags;
};
struct SceneScriptHitObject {
    u8 unknown0[0x4A];
    u16 paletteBank;
    u8 unknown4C[0x24];
    u16 palette[16];
    u8 unknown90[0x5E];
    s16 hitCount;
    u16 unknownF0, flashTimer, flashPeriod;
};
void Sound_PlaySong(u32);

void SceneScriptObject_ApplyHitFlash(struct SceneScriptHitSource *source, void *unused,
                                     struct SceneScriptHitObject *object)
{
    u8 scratch[4]; /* unused; the original reserves a 4-byte frame */
    struct {
        struct SceneScriptHitObject *object;
    } context;

    context.object = object;
    if (!(source->flags & 0x40))
        --context.object->hitCount;
    if (context.object->hitCount > 0) {
        context.object->flashTimer = 6;
        context.object->flashPeriod = 15;
        context.object->paletteBank = 0x192;
        CpuSet(gPaletteBankData + 0x3240, context.object->palette, 0x04000008);
        Sound_PlaySong(0x13E);
    }
}
