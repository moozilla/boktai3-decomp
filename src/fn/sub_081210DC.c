#include "global.h"
void Character_UpdateTimedEffect(void *);
void RegistryA8CB_UpdateAnimation(u8 *s)
{
    u32 v;
    Character_UpdateTimedEffect(s + 0x68);
    if (s == 0 || ((v = *(u16 *)(s + 0x8ba)) & 0x14) == 0) {
        (*(u32 *)(s + 0x8b0))++;
    }
}
