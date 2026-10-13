#include "global.h"
void sub_0821D6D0(void *);
void sub_08219D38(void *);
void sub_08214514(void *);
void sub_0821A2AC(int);
int SceneObjects_Destroy(u8 *p)
{
    int i;
    for (i = 0; i < *(u16 *)(p + 0x1a); i++) {
        u8 *s = *(u8 **)(p + 0x42c) + i * 140;
        if (*(void **)(s + 0x2c)) {
            sub_0821D6D0(*(void **)(s + 0x2c));
            sub_08219D38(*(void **)(s + 0x2c));
        }
        sub_08214514(s);
    }
    if (*(int *)(p + 0x34)) {
        sub_08219D38(*(void **)(p + 0x410));
        sub_08219D38(*(void **)(p + 0x414));
        sub_08219D38(*(void **)(p + 0x418));
        sub_08219D38(*(void **)(p + 0x41c));
        sub_08219D38(*(void **)(p + 0x420));
        sub_08219D38(*(void **)(p + 0x424));
        sub_08219D38(*(void **)(p + 0x428));
    }
    sub_08219D38(*(void **)(p + 0x42c));
    sub_0821A2AC(0x85e6);
    return 0;
}
