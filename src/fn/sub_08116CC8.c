#include "global.h"
#include "glove.h"
void Glove_OnActionComplete(struct Glove *s)
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
