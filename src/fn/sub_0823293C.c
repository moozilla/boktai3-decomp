#include "global.h"
struct N {
    u8 p[0x7c];
    struct N *next;
};
struct L {
    u8 p[0x1c];
    struct N *head;
};
extern u32 gUnk_02000474;
void ColliderSprite_Destroy(void *);
u32 ScriptSpriteManager_Destroy(struct L *l)
{
    struct N *p = l->head;
    while (p) {
        struct N *n = p->next;
        ColliderSprite_Destroy(p);
        p = n;
    }
    return gUnk_02000474 = 0;
}
