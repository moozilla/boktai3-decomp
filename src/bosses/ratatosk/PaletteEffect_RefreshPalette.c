#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/video/render.h"
#include "gba/macro.h"

s32 ActorResource_GetFixedId(struct Ratatosk *);

static inline void Ratatosk_SelectPalette(struct Ratatosk *self, s32 bank)
{
    if (bank == 0) {
        self->palette = self->paletteAlternate;
    } else if (bank == 501) {
        self->palette = self->paletteAlternate2;
    } else {
        CpuCopy32(gPaletteBankData + bank * 32, self->paletteBase, 32);
        self->palette = self->paletteBase;
    }
}

void PaletteEffect_RefreshPalette(struct Ratatosk *self)
{
    if (self->blendMode) {
        self->blendStep = 0;
        self->blendMode = 0;
        self->blendBank = ActorResource_GetFixedId(self);
        Ratatosk_SelectPalette(self, ActorResource_GetFixedId(self));
    }
}
