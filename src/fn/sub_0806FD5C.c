#include "scene_actor_registry.h"
#define SceneActorRegistry_DispatchMessages sub_0806FD5C
s32 sub_0821A3E8(u32, struct SceneActorRegistryMessage **);
void sub_0806FC24(u8, void *);
void sub_0806FC64(u8, void *);
void sub_0806FD08(u8, void *, u16);
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
                    sub_0806FC24(d[2], d + 4);
                    break;
                case 2:
                    sub_0806FC64(d[2], d + 4);
                    break;
                case 3:
                    sub_0806FD08(d[2], d + 6, *(u16 *)(d + 4));
                    break;
            }
            q++;
        } while (n-- > 0);
    }
}
