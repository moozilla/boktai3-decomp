#include "global.h"
#include "area_effect.h"
struct O {
    u8 f[0x26];
    u8 n;
    u8 g;
    u8 s;
};
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void AreaEffect_DisableEntryFromScript(void)
{
    struct O *o = gAreaEffectManager;
    if (o != 0) {
        if (Script_SeekToKeyword(0x6e) != 0) {
            u32 v = Script_GetValue();
            s32 i;
            for (i = 0; i < o->n; i++) {
                u32 off = i * 0xa8;
                u8 *e = (u8 *)o + off;
                u16 *id = (u16 *)(e + 0xc4);
                if (*id != 0 && *id == v) {
                    id++;
                    *(u8 *)id = 0;
                    if (e[0xc9] != 0) {
                        u8 *b = (u8 *)o + 0x34;
                        u32 *fl = (u32 *)(b + off);
                        *fl |= 1;
                    }
                    break;
                }
            }
        }
    }
}
