#include "objects/floor_switch/floor_switch_layout.h"
#define gSceneContactFlags gEngineFlags
#include "global.h"
#include "engine/engine.h"
#include "player/player.h"
struct FloorSwitchObject {
    u32 id, flags;
    u8 kind;
    u8 pad9[3];
    s16 x, y, z;
    u8 pad12[0x44 - 0x12];
    struct FloorSwitchObject *next;
};
struct FloorSwitchObjectList {
    u8 pad0[0x18];
    struct FloorSwitchObject *head;
};
struct FloorSwitchPlayer {
    u8 pad0[0x418];
    u8 state;
};
struct FloorSwitchObjectList *SceneBody_GetRegistry(void);
void FloorSwitch_SetOccupant(struct FloorSwitch *, u32);
void FloorSwitch_Release(struct FloorSwitch *);
struct FloorSwitchDelta {
    s16 x, y, z;
};
void FloorSwitch_UpdateOccupancy(struct FloorSwitch *p)
{
    struct FloorSwitchObjectList *list = SceneBody_GetRegistry();
    struct FloorSwitchObject *o;
    struct FloorSwitchDelta delta;
    s32 found;
    if (!list)
        return;
    found = 0;
    if (p->field8A)
        p->field8A = 0;
    for (o = list->head; o; o = o->next) {
        if (o->flags & 2)
            continue;
        if (!(gEngineFlags & 0x800) && !o->kind) {
            if (!(*(struct FloorSwitchPlayer **)gPlayers) ||
                (*(struct FloorSwitchPlayer **)gPlayers)->state == 3)
                continue;
        }
        {
            struct FloorSwitchDelta *d = &delta;
            struct FloorSwitchDelta *src = (struct FloorSwitchDelta *)&o->x;
            struct FloorSwitchDelta *base = (struct FloorSwitchDelta *)&p->x;
            d->x = src->x - base->x;
            d->y = src->y - base->y;
            d->z = src->z - base->z;
            if ((u16)d->x <= 256 && d->y <= 32 && d->y >= 0 && d->z <= 256 && d->z >= 0) {
                FloorSwitch_SetOccupant(p, o->id);
                if (p->field8C < p->field8E - 1)
                    p->field8C++;
                return;
            }
        }
    }
    if (!found) {
        if (p->field8C >= p->field8E)
            FloorSwitch_Release(p);
        else
            p->field8C++;
    }
}
