#include "global.h"
void Script_Call(u32, u32);
struct S {
    u8 f[0x40];
    u32 a;
    u32 b;
};
void RegistryE7BC_OnObjectComplete(struct S *p)
{
    if (p) {
        Script_Call(p->a, 0);
        p->b = 0;
    }
}
