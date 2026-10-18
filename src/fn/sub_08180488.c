#include "global.h"
#define SceneActorAttachment_MarkContact sub_08180488
struct SceneActorAttachment {
    u8 pad0[0x5a];
    u16 contact;
};
void sub_08180488(void *source, void *collider, struct SceneActorAttachment *p)
{
    p->contact = 1;
}
