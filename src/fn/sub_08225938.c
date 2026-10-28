#include "global.h"
#include "scene_body_registry.h"
struct N {
    u8 pad[0x40];
    struct N *prev;
    struct N *next;
};
s32 SceneBody_FindByPointer(struct N *);
s32 SceneBody_Unregister(struct N *n)
{
    struct SceneBodyRegistry *h = gSceneBodyRegistry;
    if (h == 0 || n == 0 || SceneBody_FindByPointer(n) == 0)
        return 0;
    {
        if (n->prev == 0)
            h->first = n->next;
        else
            n->prev->next = n->next;
        if (n->next == 0)
            h->last = n->prev;
        else
            n->next->prev = n->prev;
    }
    return 1;
}
