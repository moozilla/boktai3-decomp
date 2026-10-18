#define ActorPalette_DisableCallbacks sub_08182440
#define ActorPalette_ReturnImmediately sub_0818243C
#include "global.h"
struct ActorPaletteCallbacks { void (*update)(void); void (*cleanup)(void); };
struct ActorPaletteOwner { u8 unknown0[0x650]; struct ActorPaletteCallbacks palettes; };
void sub_0818243C(void);
void sub_08182440(struct ActorPaletteOwner *p)
{
    struct ActorPaletteCallbacks *palettes = &p->palettes;
    palettes->update = sub_0818243C;
    palettes->cleanup = sub_0818243C;
}
