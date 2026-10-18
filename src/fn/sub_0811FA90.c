#include "global.h"
struct S {
    u8 f[0x48];
    u8 pad[5];
    u8 b;
};
struct T {
    u8 f[8];
    u32 v;
};
void Memory_Clear(void *, u32);
void ScriptRecord_FindByteModifier(void *, u32);
void Modifier_InitSourceInfo(u8 *s, u32 v)
{
    u8 *a = s + 0x48;
    struct T *t = (struct T *)(s + 0x518);
    t->v = v;
    Memory_Clear(t, 8);
    ScriptRecord_FindByteModifier(t, a[5]);
}
