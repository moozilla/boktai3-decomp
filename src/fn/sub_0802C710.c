#include "global.h"
#include "ram.h"
#include "player.h"
struct Object {
    u8 pad0[0x50];
    u32 flags;
    u8 pad54[0xc0];
    void (*update)(void *);
    void (*destroy)(void *);
    u8 pad11c[0x2b];
    u8 slot;
    u8 pad148[0xd0];
    struct Object *next, *prev;
};
struct Manager {
    u8 pad0[0x18];
    void *targets[4];
    struct Object *head;
    u32 ticks, slot, serial;
};
int ActorPool_UnlinkObject(struct Manager *, struct Object *);
void Heap_Free(void *);
void ActorPool_RefreshTargets(struct Manager *);
void ActorPool_RefreshTargets(struct Manager *m)
{
    int i;
    void **out = m->targets;
    u8 **src = gPlayers;
    for (i = 0; i < 4; i++) {
        u8 *p = *src;
        *out = p ? p + 0x24 : 0;
        out++;
        src++;
    }
}
