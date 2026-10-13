#include "global.h"
void sub_08033778(int);
void Anim_Remove(void *);
int MessageMenu_Destroy(u8 *m)
{
    int *handles = (int *)(m + 0x444);
    int i = 2;
    do {
        sub_08033778(*handles++);
    } while (--i >= 0);
    Anim_Remove(m + 0x478);
    return 0;
}
