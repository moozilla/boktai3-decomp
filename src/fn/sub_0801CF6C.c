#include "global.h"
typedef struct {
    s16 x, y, z, pad;
} Vec;
int sub_0821ABA8(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
void sub_08225F88(void *);
int ResultMenu_ReadScript(u8 *p)
{
    int found, result;
    *(u16 *)(p + 0x7ac) = sub_0821ABA8(0x6e, 0x6740);
    *(u16 *)(p + 0x7ae) = 5;
    found = Script_SeekToKeyword(0x70);
    if (found) {
        *(s16 *)(p + 0x18) = Script_GetValue();
        *(s16 *)(p + 0x1a) = Script_GetValue();
        *(s16 *)(p + 0x1c) = Script_GetValue();
    } else {
        *(s16 *)(p + 0x18) = found;
        *(s16 *)(p + 0x1a) = found;
        *(s16 *)(p + 0x1c) = found;
    }
    {
        Vec *position = (Vec *)(p + 0x18);
        *(Vec *)(p + 0x20) = *(Vec *)(p + 0x18);
        sub_08225F88(position);
    }
    *(u16 *)(p + 0x7a0) = sub_0821ABA8(0x65, 0);
    p[0xae] = sub_0821ABA8(0x6c, 0);
    if (!Script_SeekToKeyword(0x73))
        result = -1;
    else {
        p[0xba] = Script_GetValue();
        p[0xbb] = Script_GetValue();
        p[0xbc] = sub_0821ABA8(0x62, 0);
        result = 0;
    }
    return result;
}
