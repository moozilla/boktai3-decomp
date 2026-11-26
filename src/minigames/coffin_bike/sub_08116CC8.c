#include "global.h"
#include "minigames/coffin_bike/coffin_bike.h"
void CoffinBike_OnActionComplete(struct Glove *s)
{
    u8 *p = &s->active;
    u16 *h;
    u16 m;
    if (*p != 0) {
        *p = 0;
        Script_Call(s->localScript, 0);
    }
    m = 1;
    h = &s->flags;
    *h = *h | m;
    s->timer++;
}
