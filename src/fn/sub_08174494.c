#include "global.h"
void TextLabel_DisplaySuffix(u32, s32, void *, void *);
void Presentation_ApplyIndexedGraphicData(s32 index, void *data, void *target)
{
    TextLabel_DisplaySuffix(1, index, data, target);
}
