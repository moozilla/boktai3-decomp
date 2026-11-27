#include "global.h"
struct SceneActorAttachment {
    u8 pad0[0x5a];
    u16 contact;
};
void SceneActorAttachment_MarkContact(void *source, void *collider, struct SceneActorAttachment *p)
{
    p->contact = 1;
}
