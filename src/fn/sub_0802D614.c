#include "global.h"
struct Object {
    u8 pad[0x2b8];
    void *data;
    void *entry;
};
void *Actor_FindResourceData(int, int);
void *Actor_FindResourceEntry(int, int);
int Actor_InitResources(void *unused, struct Object *p, int type, int variant)
{
    p->data = Actor_FindResourceData(type, variant);
    if (p->data) {
        p->entry = Actor_FindResourceEntry(type, variant);
        if (p->entry)
            return 0;
    }
    return -1;
}
