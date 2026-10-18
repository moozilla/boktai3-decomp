#include "global.h"
struct Entry {
    u8 kind;
    s8 stride;
    u16 animation;
};
struct Actor {
    u8 pad[0x2b8];
    struct Entry *entries;
};
int Actor_SetFourWayAnimation(struct Actor *, int, int, int, int, int);
int Actor_SetMirroredFourWayAnimation(struct Actor *, int, int, int, int, int);
int Actor_SetFrontBackAnimation(struct Actor *, int, int, int, int, int);
int Actor_SetMirroredEightWayAnimation(struct Actor *, int, int, int, int, int);
int Actor_SetUndirectedAnimation(struct Actor *, int, int, int, int, int);
int Actor_PlayAnimationEntry(struct Actor *p, int id, int a, int b, int c)
{
    struct Entry *entry = &p->entries[id];
    int result;
    if (entry->kind == 1)
        result = Actor_SetFourWayAnimation(p, entry->animation, a, entry->stride, b, c);
    else if (entry->kind == 3)
        result = Actor_SetMirroredFourWayAnimation(p, entry->animation, a, entry->stride, b, c);
    else if (entry->kind == 2)
        result = Actor_SetFrontBackAnimation(p, entry->animation, a, entry->stride, b, c);
    else if (entry->kind == 4)
        result = Actor_SetMirroredEightWayAnimation(p, entry->animation, a, entry->stride, b, c);
    else
        result = Actor_SetUndirectedAnimation(p, entry->animation, a, entry->stride, b, c);
    return result;
}
