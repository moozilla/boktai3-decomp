#include "global.h"
struct Animation {
    u8 pad[12];
    u16 timer;
    u8 flag;
};
struct P {
    u8 pad[0x85c];
    int countdown;
    struct Animation animation;
};
int sub_0803C070(struct P *p)
{
    int alias;
    struct Animation *animation = &p->animation;
    alias = 0;
    if (p->countdown > alias) {
        u32 zero;
        p->countdown--;
        zero = alias;
        animation->timer = alias;
        animation->flag = zero;
        return alias;
    }
    if (animation->timer) {
        return 1;
    }
    return alias;
}
