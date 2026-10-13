#include "global.h"
u8 *sub_0821A2C4(int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
void SceneObject_AnimateActive(void);
void SceneObjects_ScriptPromote(void)
{
    u8 *p = sub_0821A2C4(0x85e6);
    int id;
    int i;
    if (!p)
        return;
    if (!Script_SeekToKeyword(0x6e))
        return;
    id = Script_GetValue();
    i = 0;
    if (i < *(u16 *)(p + 0x1a)) {
        u8 **base = (u8 **)(p + 0x42c);
        int zero;
        int two;
        zero = 0;
        two = 2;
        do {
            u8 *s = *base + i * 140;
            if (*(u16 *)(s + 0x34) == id) {
                s[0x41] = 1;
                {
                    int offset = p[0x1c] * 2;
                    u8 *ids;
                    ids = p;
                    ids += 0x1e;
                    ids += offset;
                    *(u16 *)ids = *(u16 *)(s + 0x34);
                }
                p[0x1c]++;
                s[0x37] = two;
                *(void **)(s + 0x30) = SceneObject_AnimateActive;
                s[0x39] = zero;
                s[0x3a] = zero;
            }
            i++;
        } while (i < *(u16 *)(p + 0x1a));
    }
}
