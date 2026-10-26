#include "global.h"
#include "render.h"

void Bg_SetTileUpload(u32 i, u32 a, u16 b, u16 c)
{
    gBgTileUploadSrc[i] = a;
    gBgTileUploadParamA[i] = c;
    gBgTileUploadParamB[i] = b;
}
