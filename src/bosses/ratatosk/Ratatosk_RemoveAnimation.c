#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

u32 Anim_Remove(void *);

u32 Ratatosk_RemoveAnimation(struct Ratatosk *p)
{
    return Anim_Remove(p->animationData);
}
