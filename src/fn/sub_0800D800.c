#include "falling_objects.h"

s32 sub_0800D800(struct FallingObjectManager *p, void *unused)
{
    gUnk_0200046C = p;
    p->animation = ScriptProc_Start(0x922E, 0x931E);
    p->head = 0;
    return 0;
}
