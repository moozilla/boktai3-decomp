#include "global.h"
// Tentative: Spread target searches across the manager's four update slots.
struct Vec {
    s16 x, y, z, pad;
};
struct Object {
    u8 pad0[9];
    u8 angle;
    u8 pada[2];
    struct Vec pos;
    u8 pad14[0x124];
    u32 range;
    u8 arc, pad13d[3];
    u32 distance;
    u8 delta;
    s8 target;
    u8 count, slot;
};
void *ActorPool_GetManager(void);
struct Object *Actor_GetPartyTarget(int);
int Math_Atan2(int, int);
struct Manager {
    u8 pad[0x30];
    u32 slot;
};
int Actor_FindTargetInFacingArc(struct Object *);
void Actor_RefreshTarget(struct Object *);
void Actor_UpdateSelectedTarget(struct Object *p)
{
    struct Manager *m = ActorPool_GetManager();
    int selected = 0;
    if (m->slot == p->slot)
        selected = 1;
    if (selected) {
        if (p->target >= 0)
            Actor_RefreshTarget(p);
        else
            p->target = Actor_FindTargetInFacingArc(p);
    }
}
