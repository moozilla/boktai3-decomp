#include "global.h"
#include "minigames/coffin_bike/coffin_bike.h"
void CoffinBike_UpdateStopping(struct CoffinBike *);
void CoffinBike_BeginStopping(struct CoffinBike *s)
{
    void (*f)(void *) = (void (*)(void *))CoffinBike_UpdateStopping;
    u8 *p = &s->active;
    u32 z = 0;
    *p = 1;
    s->update = f;
    s->timer = z;
}
