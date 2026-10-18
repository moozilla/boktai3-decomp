#include "global.h"
extern u8 *gUnk_030053F8;
u8 Save_WriteMainState(s32);
u8 Save_WriteContinuation(s32);
u8 Save_WriteHeader(void);
/* Switch to the other of two slots (0/1) if both checks accept it. */
u8 Save_WriteAlternateSlotAndSelect(void)
{
    s32 other = 1 - gUnk_030053F8[8];
    if (Save_WriteMainState(other) && Save_WriteContinuation(other)) {
        gUnk_030053F8[8] = other;
        return Save_WriteHeader();
    }
    return 0;
}
