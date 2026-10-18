#include "global.h"

void Actor_UpdateDrawable(void);
void Actor_UpdateSelectedTarget(void *);
void Actor_UpdateDrawableAndTarget(void *p)
{
    Actor_UpdateDrawable();
    Actor_UpdateSelectedTarget(p);
}
