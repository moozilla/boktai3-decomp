#include "global.h"
extern u8 *gUnk_02000258;
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);

u8 sub_081B8104(void)
{
    u8 *m = gUnk_02000258;
    if (m != NULL) {
        s32 id;
        s32 i;
        if (Script_SeekToKeyword('i'))
            id = Script_GetValue();
        else
            id = 0;
        for (i = 0; i < m[0x18]; i++) {
            if (*(s16 *)(m + i * 0x118 + 0x14c) == id)
                return m[i * 0x118 + 0x15e];
        }
    }
    return 0;
}
