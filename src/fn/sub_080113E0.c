#include "message_menu.h"
int MessageMenu_Destroy(struct MessageMenu *m)
{
    int *handles = &m->mainText;
    int i = 2;
    do {
        sub_08033778(*handles++);
    } while (--i >= 0);
    Anim_Remove(m->transition);
    return 0;
}
