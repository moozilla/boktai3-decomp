#include "global.h"
#include "gba/io_reg.h"
#include "engine/video/render.h"

void Palette_UploadBuffers(void)
{
    DmaCopy32(3, gBgPaletteSource, BG_PLTT, BG_PLTT_SIZE);
    DmaCopy32(3, gObjPaletteBuffer, OBJ_PLTT, OBJ_PLTT_SIZE);
    if (gBackdropOverride == 0) {
        *(u16 *)BG_PLTT = 0x1084;
    } else {
        *(u16 *)BG_PLTT = gBackdropColor;
        gBackdropColor = 0x1084;
        gBackdropOverride = 0;
    }
}
