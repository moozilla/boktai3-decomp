#include "global.h"
typedef struct {
    u8 pad[10], available, tail[5];
} Item;
extern Item gUnk_08E60F80[];
int sub_0822E104(int);
int sub_0822E224(int);
void SelectionMenu_ItemAvailability(u8 *arg)
{
    u8 *p = arg;
    int i;
    int index;
    u8 alias;
    int mask;
    *((u32 *)(p + 0x34)) = 0xffff;
    i = 0;
    mask = 1;
    /* do-while(0) only keeps agbcc's block layout */
    do {
        index = sub_0822E104(i);
        if (index >= 0) {
            Item *table = gUnk_08E60F80;
            alias = table[index].available;
            if (alias && (!sub_0822E224(i))) {
                goto next;
            }
        }
        if (p) {
            *((u32 *)(p + 0x34)) &= ~(mask << i);
        } else {
            *((u32 *)(p + 0x34)) &= ~(mask << i);
        }
    next:
        i++;

    } while (i < 16);
}
