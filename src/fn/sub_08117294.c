#include "global.h"
#include "glove.h"
void Glove_UpdateStopping(struct Glove *);
void Glove_BeginStopping(struct Glove *s)
{
    void (*f)(void *) = (void (*)(void *))Glove_UpdateStopping;
    u8 *p = &s->active;
    u32 z = 0;
    *p = 1;
    s->update = f;
    s->timer = z;
}
