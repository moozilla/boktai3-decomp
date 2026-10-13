// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObjects_ScriptAppend sub_0801E3E4
typedef struct {
    s16 x, y, z, pad;
} Vec;
void *sub_0821A2C4(int);
int sub_0821ABA8(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
void sub_0821E4D4(u16, void *);
int sub_0801E184(void *, u16, void *, int, u8, u8, u8, int, int, int, int, void *, u8, int);
int SceneObjects_ScriptAppend(void)
{
    int data[4];
    Vec position;
    void *p;
    u16 id;
    int value;
    u8 kind, variant, orientation;
    int type, field45, field88;
    u8 field44;
    int flags, field41, i, found;
    register int *out asm("r5");
    p = sub_0821A2C4(0x85e6);
    if (!p)
        return -1;
    id = sub_0821ABA8(0x6e, 0);
    value = sub_0821ABA8(0x64, 0);
    kind = sub_0821ABA8(0x66, 0);
    variant = sub_0821ABA8(0x73, 0);
    orientation = sub_0821ABA8(0x72, 0);
    type = sub_0821ABA8(0x76, 0);
    field45 = sub_0821ABA8(0x63, 0);
    field88 = sub_0821ABA8(0x68, 400);
    field44 = sub_0821ABA8(0x6d, 0);
    if (Script_SeekToKeyword(0x6b)) {
        flags = Script_GetValue();
        field41 = Script_GetValue();
        {
            register Vec *pos asm("r6") = &position;
            asm("" : : "r"(pos));
        }
        out = data;
        for (i = 3; i >= 0; i--)
            *out++ = Script_GetValue();
    } else {
        flags = 0;
        field41 = 0;
    }
    if (Script_SeekToKeyword(0x74))
        sub_0821E4D4(Script_GetValue(), &position);
    else {
        found = Script_SeekToKeyword(0x70);
        if (found) {
            position.x = Script_GetValue();
            position.y = Script_GetValue();
            position.z = Script_GetValue();
        } else {
            position.x = found;
            position.y = found;
            position.z = found;
        }
    }
    return sub_0801E184(p, id, &position, value, kind, variant, orientation, type, field88, flags,
                        field41, data, field44, field45);
}
