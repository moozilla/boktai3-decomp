#include "global.h"
#include "render.h"

void Bg_ResetScale(u32 n)
{
    struct BackgroundLayer *bg = &gBgRecords[n];
    bg->scaleX = 0x1000;
    bg->scaleY = 0x1000;
}
