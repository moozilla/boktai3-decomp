#include "global.h"
struct Actor { u32 id; u8 pad[64]; struct Actor *next; };
struct Context { u8 pad[24]; struct Actor *actors; };
extern struct Context *gUnk_030025F8;
struct Actor *sub_082258DC(u16 id)
{
    struct Actor *actor;
    if (!gUnk_030025F8) return 0;
    actor = gUnk_030025F8->actors;
    while (actor) {
        if (actor->id == id) return actor;
        actor = actor->next;
    }
    return 0;
}
