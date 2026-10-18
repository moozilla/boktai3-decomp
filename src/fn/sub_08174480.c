#include "global.h"
void TextLabel_DisplaySuffix(u32, s32, void *, void *);
void Presentation_ApplyGraphicData(void *data, void *target)
{
    TextLabel_DisplaySuffix(0, -1, data, target);
}
