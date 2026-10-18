#define ActorPalette_BeginTransition sub_08182064
#define ActorPalette_RestoreSelectedBanks sub_081816B0
#include "global.h"
struct ActorPaletteCallbacks { void (*update)(void *); void (*cleanup)(void *); };
struct ActorPaletteOwner {
    u8 unknown0[0x650]; struct ActorPaletteCallbacks palettes;
    u8 unknown658[0x480]; u16 timer;
};
void sub_08181E78(void *);
void sub_081816B0(void *);
void sub_08182064(struct ActorPaletteOwner *p)
{
    struct ActorPaletteCallbacks *palettes = &p->palettes;
    void (*update)(void *) = sub_08181E78;
    void (*cleanup)(void *) = sub_081816B0;
    palettes->update = update;
    palettes->cleanup = cleanup;
    p->timer = 0;
}
