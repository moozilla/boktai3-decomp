#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Animation {u8 pad[12];u16 timer;u8 flag;};
struct P {
    u8 pad[0x85c];
    int countdown;
    struct Animation animation;
};
int sub_0803C070(struct P *p)
{
    struct Animation *animation=&p->animation;
    if(p->countdown>0) {
        register u32 zero asm("r1");
        p->countdown--;
        zero=0;
        asm("" : "+r"(zero));
        animation->timer=0;
        animation->flag=zero;
        return 0;
    }
    if(animation->timer)return 1;
    return 0;
}
