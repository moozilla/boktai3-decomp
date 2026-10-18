#include "global.h"
extern u8 *gUnk_02000488;
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, s32);
void TetherActor_WriteTransformToScript(void)
{
    u8 *p = gUnk_02000488;
    u32 buf[2];
    if (p) {
        Script_ReadReference(buf);
        Script_WriteReferenceValue(buf, 0, *(s16 *)(p + 0x50));
        Script_ReadReference(buf);
        Script_WriteReferenceValue(buf, 0, *(s16 *)(p + 0x52));
        Script_ReadReference(buf);
        Script_WriteReferenceValue(buf, 0, *(s16 *)(p + 0x54));
        Script_ReadReference(buf);
        Script_WriteReferenceValue(buf, 0, *(u8 *)(p + 0x260));
    }
}
