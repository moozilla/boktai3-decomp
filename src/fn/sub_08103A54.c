#include "named_object.h"

s32 NamedObject_InitManager(struct NamedObjectManager *p)
{
    p->sequences = ScriptProc_Start(0x922E, 0x13F9);
    gNamedActorManager = p;
    p->used = 0;
    return 0;
}
