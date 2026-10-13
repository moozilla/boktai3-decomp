#include "global.h"
struct Effect {
    u8 pad[4];
    u8 state;
    u8 rest[127];
};
struct Manager {
    u8 pad[24];
    u32 enabled;
    u8 pad1c[32];
    struct Effect effects[8];
};
extern void (*gUnk_08604C64[])(struct Manager *, struct Effect *, int);
int FloatingEffectManager_Update(struct Manager *m)
{
    int i = 0;
    void (**table)(struct Manager *, struct Effect *, int) = gUnk_08604C64;
    u8 *state;
    struct Effect *p;
    state = &m->effects[0].state;
    p = m->effects;
    for (; i <= 7; state += 132, p++, i++) {
        if (m->enabled & (1 << i)) {
            if (*state <= 6)
                table[*state](m, p, i);
        }
    }
}
