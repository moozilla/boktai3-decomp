#include "falling_objects.h"

void sub_08013B74(void *effect);

void sub_0800CC34(void *object)
{
    struct FallingObject *p = object;
    if (p->effectActive) {
        sub_08013B74(p->effect);
        p->effectActive = 0;
    }
}
