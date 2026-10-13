#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Object {
    u8 active;
    u8 pad[0x2cf];
    void (*callbacks[4])(void *, struct Object *);
};
int sub_0802D9C0(void *context, struct Object *p)
{
    register int i asm("r6");
    void (**fn)(void *, struct Object *);
    if (!p->active)
        return -1;
    i = 0;
    fn = p->callbacks;
    do {
        if (*fn)
            (*fn)(context, p);
        fn++;
        i++;
    } while (i < 4);
    return 0;
}
