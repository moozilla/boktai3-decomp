#include "global.h"
extern u8 *gUnk_02000580[4];
s32 Script_GetOptionalIValue(void);
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);

void sub_08243704(void)
{
    s32 i = Script_GetOptionalIValue();
    if (gUnk_02000580[i] != NULL) {
        if (Script_SeekToKeyword('T'))
            Script_GetValue();
    }
}
