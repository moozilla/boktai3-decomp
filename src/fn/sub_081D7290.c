#include "global.h"
void BestiaryDisplay_Hide(u32);
void TextSequence_OnDestroy(u8 *p)
{
    BestiaryDisplay_Hide(*(u32 *)(p + 0x318));
}
