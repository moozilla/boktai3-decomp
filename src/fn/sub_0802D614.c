#include "global.h"
struct Object {
    u8 pad[0x2b8];
    void *data;
    void *entry;
};
void *sub_0802D5B4(int, int);
void *sub_0802D5E0(int, int);
int sub_0802D614(void *unused, struct Object *p, int type, int variant)
{
    p->data = sub_0802D5B4(type, variant);
    if (p->data) {
        p->entry = sub_0802D5E0(type, variant);
        if (p->entry)
            return 0;
    }
    return -1;
}
