#include "global.h"
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, u32);
extern u8 gUnk_03005430[];
void Clock_GetWeekdayFromScript(void)
{
    u32 buf[2];
    Script_ReadReference(buf);
    Script_WriteReferenceValue(buf, 0, gUnk_03005430[8]);
}
