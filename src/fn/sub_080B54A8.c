#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 Vorn_Init(u8 *);
void sub_080B43B0(u8 *);
void Heap_Free(u8 *);

void Vorn_Create(void)
{
    u8 *p = Heap_Alloc(0x720);
    if (p != 0) {
        Memory_Clear(p, 0x720);
        if (Vorn_Init(p) < 0) {
            sub_080B43B0(p);
            Heap_Free(p);
        }
    }
}
