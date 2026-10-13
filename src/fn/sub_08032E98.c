#include "global.h"
struct Vm {
    u8 pad[0xC8];
    u32 choices[16];
    u8 pad108[0x48];
    u32 count;
    u8 selected[32];
};
void Script_Call(u32, int);
void sub_08032E98(struct Vm *p)
{
    u32 i = 0;
    for (; i < p->count; i++)
        Script_Call(p->choices[p->selected[i]], 0);
}
