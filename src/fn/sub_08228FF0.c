#include "global.h"
extern u8 gUnk_03005430[];
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, int, int);
void SunClock_GetSunsetSpanFromScript(void)
{
    u32 v[2];
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gUnk_03005430[12]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gUnk_03005430[13]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gUnk_03005430[16]);
    Script_ReadReference(v);
    Script_WriteReferenceValue(v, 0, gUnk_03005430[17]);
}
