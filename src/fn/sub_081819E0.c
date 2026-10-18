#include "global.h"
struct ActorPaletteCallbacks {
    void *update;
    void *cleanup;
};
struct ActorPaletteOwner {
    u8 unknown0[0x650];
    struct ActorPaletteCallbacks palettes;
};
s32 Callback_InvokeActor(void *, void *);
void ActorPalette_RestoreSelectedBanks(void *);
void ActorPalette_Reset(struct ActorPaletteOwner *p)
{
    struct ActorPaletteCallbacks *palettes = &p->palettes;
    palettes->update = 0;
    if (palettes->cleanup) {
        Callback_InvokeActor(palettes, palettes->cleanup);
        palettes->cleanup = 0;
    }
    ActorPalette_RestoreSelectedBanks(palettes);
}
