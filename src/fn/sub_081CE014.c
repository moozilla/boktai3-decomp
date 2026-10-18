#include "global.h"
void Actor_UpdateVisibilityAndShadow(void);
void sub_081CDE14(u8 *, u8 *);
void sub_081CDEF8(u8 *, u8 *);
s32 SceneCharacter_UpdateDisplay(u8 *a, u8 *b)
{
    Actor_UpdateVisibilityAndShadow();
    if (b[1] == 0x1d || b[1] == 9) {
        sub_081CDE14(a, b);
        sub_081CDEF8(a, b);
    }
    return 0;
}
