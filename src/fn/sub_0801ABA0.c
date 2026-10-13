#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define SelectionMenu_ItemAvailability sub_0801ABA0
typedef struct {
    u8 pad[10], available, tail[5];
} Item;
extern Item gUnk_08E60F80[];
int sub_0822E104(int);
int sub_0822E224(int);
void SelectionMenu_ItemAvailability(u8 *arg)
{
    register u8 *p asm("r4") = arg;
    int i, index, mask;
    *(u32 *)(p + 0x34) = 0xffff;
    i = 0;
    mask = 1;
    do {
        index = sub_0822E104(i);
        if (index >= 0) {
            register Item *table asm("r0") = gUnk_08E60F80;
            if (table[index].available && !sub_0822E224(i))
                goto next;
        }
        *(u32 *)(p + 0x34) &= ~(mask << i);
    next:
        i++;
    } while (i < 16);
}
