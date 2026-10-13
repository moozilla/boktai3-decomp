#include "global.h"
struct Object {
    u8 active;
    u8 pad1[0x5b];
    u8 motion[0x78];
    u8 drawable[1];
};
extern void *gUnk_02000480;
void sub_0802D688(void *, struct Object *);
void sub_0802D914(struct Object *);
void sub_08225938(void *);
void sub_08042558(void *);
void sub_0802D85C(struct Object *);
void Heap_Free(void *);
int sub_0802D96C(struct Object *p)
{
    int result;
    if (gUnk_02000480 && p->active) {
        sub_0802D688(gUnk_02000480, p);
        sub_0802D914(p);
        sub_08225938(p->motion);
        sub_08042558(p->drawable);
        sub_0802D85C(p);
        p->active = 0;
        Heap_Free(p);
        result = 0;
    } else
        result = -1;
    return result;
}
