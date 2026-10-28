#include "global.h"
#include "area_effect_link.h"
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, s32, s32);
static inline u8 *getBlock(void)
{
    u8 *p = gLinkSession;
    if (p) {
        u8 state = p[0x53];
        p += 0xE0;
        if (state == 1)
            goto done;
    }
    p = 0;
done:
    return p;
}
/* Registry routine 1022: stores byte +0x15D of the object at 0x020004B4 in
   the result variable while its +0x53 is 1, else 0 (meaning unknown). */
void Registry1022_GetValueFromScript(void)
{
    u32 result[2];
    u8 *block;
    s32 value;
    Script_ReadReference(result);
    block = getBlock();
    value = 0;
    if (block)
        value = block[0x7D];
    Script_WriteReferenceValue(result, 0, value);
}
