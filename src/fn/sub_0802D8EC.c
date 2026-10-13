#include "global.h"
struct Object {
    u8 pad0[0x5c];
    u8 motion[0x196];
    u8 registered;
};
int sub_08003C00(void *);
void sub_08003C10(void *);
int sub_0802D8EC(struct Object *p)
{
    int result;
    if (sub_08003C00(p->motion) >= 0) {
        p->registered = 1;
        result = 0;
    } else
        result = -1;
    return result;
}
