#include "scene_actor_registry.h"
s32 sub_0821A3E8(u32, struct SceneActorRegistryMessage **);
void SceneActorRegistry_SendBroadcast(u8, void *);
void SceneActorRegistry_SendToCategory(u8, void *);
void SceneActorRegistry_SendToActorId(u8, void *, u16);
void SceneActorRegistry_DispatchMessages(struct SceneActorRegistryManager *p)
{
    s32 n = sub_0821A3E8(p->resourceId, &p->messages);
    if (n > 0) {
        struct SceneActorRegistryMessage *q = p->messages;
        n--;
        do {
            u8 *d = q->data;
            switch (*(u16 *)d) {
                case 1:
                    SceneActorRegistry_SendBroadcast(d[2], d + 4);
                    break;
                case 2:
                    SceneActorRegistry_SendToCategory(d[2], d + 4);
                    break;
                case 3:
                    SceneActorRegistry_SendToActorId(d[2], d + 6, *(u16 *)(d + 4));
                    break;
            }
            q++;
        } while (n-- > 0);
    }
}
