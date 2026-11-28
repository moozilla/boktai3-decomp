#include "objects/floor_switch/floor_switch_layout.h"
#define gSceneContactFlags gEngineFlags
#include "global.h"
#include "engine/engine.h"
#include "player/player.h"
#include "scene/motion/scene_actor_terrain.h"
struct FloorSwitchObject {
    u32 id, flags;
    u8 kind;
    u8 pad9[3];
    struct ContactPosition position;
    u8 pad14[0x44 - 0x14];
    struct FloorSwitchObject *next;
};
struct FloorSwitchObjectList {
    u8 pad0[0x18];
    struct FloorSwitchObject *head;
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
    struct ContactPosition delta;
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
            if (!gPlayers[0] || gPlayers[0]->actionMode == 3)
                continue;
        }
        SceneActorTerrain_Subtract(&delta, &o->position, (struct ContactPosition *)&p->x);
        if ((u16)delta.x <= 256 && delta.y <= 32 && delta.y >= 0 && delta.z <= 256 &&
            delta.z >= 0) {
            FloorSwitch_SetOccupant(p, o->id);
            if (p->field8C < p->field8E - 1)
                p->field8C++;
            return;
        }
    }
    if (!found) {
        if (p->field8C >= p->field8E)
            FloorSwitch_Release(p);
        else
            p->field8C++;
    }
}
