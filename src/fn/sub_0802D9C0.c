#include "global.h"
struct Object {
    u8 active;
    u8 pad[0x2cf];
    void (*callbacks[4])(void *, struct Object *);
};
int sub_0802D9C0(void *context, struct Object *p)
{
    void (**alias)(void *, struct Object *);
    int i;
    void (**fn)(void *, struct Object *);
    if (!p->active) {
        return -1;
    }
    if (context || fn) {
        i = 0;
        fn = (alias = p->callbacks);
    } else {
        i = 0;
        fn = (alias = p->callbacks);
    }
    /* do-while(0) only keeps agbcc's block layout */
    do {
        if (*fn) {
            (*fn)(context, p);
        }
        fn++;
        i++;
    } while (i < 4);
    return 0;
}
