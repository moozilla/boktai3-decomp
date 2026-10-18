#include "global.h"
struct ActorPaletteCallbacks {
    void (*update)(void);
    void (*cleanup)(void);
};
struct ActorPaletteOwner {
    u8 unknown0[0x650];
    struct ActorPaletteCallbacks palettes;
};
void ActorPalette_ReturnImmediately(void);
void ActorPalette_DisableCallbacks(struct ActorPaletteOwner *p)
{
    struct ActorPaletteCallbacks *palettes = &p->palettes;
    palettes->update = ActorPalette_ReturnImmediately;
    palettes->cleanup = ActorPalette_ReturnImmediately;
}
