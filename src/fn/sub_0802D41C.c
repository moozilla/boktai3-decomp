#include "global.h"

void Actor_UpdateDrawableAndTarget(void *);
void Actor_DispatchStateHandler(void *);
void Actor_UpdateAction(void *);
void Actor_UpdateControllerCollisionAndTimers(void *);

void Actor_OnControllerUpdate(void *p)
{
    Actor_UpdateDrawableAndTarget(p);
    Actor_DispatchStateHandler(p);
    Actor_UpdateAction(p);
    Actor_UpdateControllerCollisionAndTimers(p);
}
