#define ActorPalette_Reset sub_081819E0
#define ActorPalette_RestoreSelectedBanks sub_081816B0
#include "global.h"
struct ActorPaletteCallbacks { void *update; void *cleanup; };
struct ActorPaletteOwner { u8 unknown0[0x650]; struct ActorPaletteCallbacks palettes; };
s32 Callback_InvokeActor(void *, void *);
void sub_081816B0(void *);
void sub_081819E0(struct ActorPaletteOwner *p)
{
    struct ActorPaletteCallbacks *palettes = &p->palettes;
    palettes->update = 0;
    if (palettes->cleanup) {
        Callback_InvokeActor(palettes, palettes->cleanup);
        palettes->cleanup = 0;
    }
    sub_081816B0(palettes);
}
