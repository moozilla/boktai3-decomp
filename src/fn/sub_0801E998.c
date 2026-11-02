// MATCHING_CONCESSION: register-pinning
#include "scene_object_slot.h"
void SceneObject_UpdateLowered(void);
int SceneObjects_ScriptLower(void)
{
    u8 *p = ResourceRegistry_GetResource(0x85e6);
    int found;
    int i;
    int id;
    if (!p)
        return -1;
    found = 0;
    if (!Script_SeekToKeyword(0x6e))
        return -1;
    id = Script_GetValue();
    i = 0;
    if (found < *(u16 *)(p + 0x1a)) {
        u8 **base = (u8 **)(p + 0x42c);
        int zero;
        int one;
        zero = 0;
        one = 1;
        do {
            u8 *s;
            s = *base + i * 140;
            if (*(u16 *)(s + 0x34) == id) {
                *(u16 *)(s + 0x82) = zero;
                s[0x37] = one;
                {
                    register void *fn asm("r2") = SceneObject_UpdateLowered;
                    *(void **)(s + 0x30) = fn;
                }
                {
                    int pose = s[0x84];
                    s[0x39] = pose;
                }
                {
                    u8 *out = s + 0x3a;
                    register int clear asm("r1");
                    clear = 0;
                    *out = clear;
                }
                ((struct SceneObjSlot *)s)->block->w6 &= ~2;
                ((struct SceneObjSlot *)s)->block->w6 |= 3;
                asm("" ::: "memory");
                ((struct SceneObjSlot *)s)->block->b4 =
                    (((struct SceneObjSlot *)s)->block->b4 & 0xf0) | (s[0x3b] + 1);
                *(u16 *)(s + 0x80) = zero;
                *(u16 *)(s + 0x1e) = *(u16 *)(s + 0x72);
                found = 1;
            }
            i++;
        } while (i < *(u16 *)(p + 0x1a));
    }
    return found;
}
