#include "global.h"
#ifdef NONMATCHING
// fuzzy: 85.4%
// foundry draft (worker best 80.4%); what differs is mostly register allocation and instruction order
#define SceneScriptObject_ApplyHitFlash sub_081B8578
#define gSpritePaletteBanks             gUnk_030042E4
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
extern u8 *gUnk_030042E4;
void Sound_PlaySong(u32);
void sub_081B8578(struct SceneScriptHitSource *source, void *unused,
                  struct SceneScriptHitObject *object)
{
    struct {
        struct SceneScriptHitObject *object;
    } context = {object};
    if (!(source->flags & 0x40))
        --context.object->hitCount;
    if (context.object->hitCount > 0) {
        context.object->flashTimer = 6;
        context.object->flashPeriod = 15;
        context.object->paletteBank = 0x192;
        CpuSet(gSpritePaletteBanks + 0x3240, context.object->palette, 0x04000008);
        Sound_PlaySong(0x13E);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081B8578);
#endif
