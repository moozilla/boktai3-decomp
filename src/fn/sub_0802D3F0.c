#include "global.h"

void Actor_DispatchActionCallback(void);

void Actor_UpdateAction(void)
{
    Actor_DispatchActionCallback();
}
