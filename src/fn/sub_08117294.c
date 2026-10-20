#include "global.h"
struct S {
    u8 f[0x864];
    u32 c;
    u8 on;
    u8 p[3];
    void (*cb)(void);
};
void Glove_UpdateStopping(void);
void Glove_BeginStopping(struct S *s)
{
    void (*f)(void) = Glove_UpdateStopping;
    u8 *p = &s->on;
    u32 z = 0;
    *p = 1;
    s->cb = f;
    s->c = z;
}
