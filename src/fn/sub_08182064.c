#include "global.h"
struct ActorPaletteCallbacks {
    void (*update)(void *);
    void (*cleanup)(void *);
};
struct ActorPaletteOwner {
    u8 unknown0[0x650];
    struct ActorPaletteCallbacks palettes;
    u8 unknown658[0x480];
    u16 timer;
};
void sub_08181E78(void *);
void ActorPalette_RestoreSelectedBanks(void *);
void ActorPalette_BeginTransition(struct ActorPaletteOwner *p)
{
    struct ActorPaletteCallbacks *palettes = &p->palettes;
    void (*update)(void *) = sub_08181E78;
    void (*cleanup)(void *) = ActorPalette_RestoreSelectedBanks;
    palettes->update = update;
    palettes->cleanup = cleanup;
    p->timer = 0;
}
