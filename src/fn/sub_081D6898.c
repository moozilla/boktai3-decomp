#include "global.h"

void Anim_Remove(u8 *);

void MonsterBook_RemoveActiveAnimation(u8 *p)
{
    if (p[4] != 0) {
        Anim_Remove(p);
    }
}
