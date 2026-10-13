#include "global.h"
void sub_0821D6D0(void *);
void Heap_Free(void *);
void Sprite_Remove(void *);
void sub_0821A2AC(int);
int SceneObjects_Destroy(u8 *p)
{
    int i;
    for (i = 0; i < *(u16 *)(p + 0x1a); i++) {
        u8 *s = *(u8 **)(p + 0x42c) + i * 140;
        if (*(void **)(s + 0x2c)) {
            sub_0821D6D0(*(void **)(s + 0x2c));
            Heap_Free(*(void **)(s + 0x2c));
        }
        Sprite_Remove(s);
    }
    if (*(int *)(p + 0x34)) {
        Heap_Free(*(void **)(p + 0x410));
        Heap_Free(*(void **)(p + 0x414));
        Heap_Free(*(void **)(p + 0x418));
        Heap_Free(*(void **)(p + 0x41c));
        Heap_Free(*(void **)(p + 0x420));
        Heap_Free(*(void **)(p + 0x424));
        Heap_Free(*(void **)(p + 0x428));
    }
    Heap_Free(*(void **)(p + 0x42c));
    sub_0821A2AC(0x85e6);
    return 0;
}
