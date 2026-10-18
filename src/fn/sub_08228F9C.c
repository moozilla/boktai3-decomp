#include "global.h"
extern u8 gUnk_03005430[];
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, int, int);
void SunClock_GetSunriseSpanFromScript(void)
{
    u32 v[2];
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gUnk_03005430[10]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gUnk_03005430[11]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gUnk_03005430[18]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gUnk_03005430[19]);
}
