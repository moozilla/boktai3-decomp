#include "named_object.h"
static inline u8 HasFlag(u32 mask, s16 *flags)
{
    if ((u16)*flags & mask)
        return TRUE;
    return FALSE;
}
static inline s32 Load(s16 *p)
{
    return *p;
}
void NamedObject_UpdateRouteFollower(struct NamedObject *o)
{
    struct NamedObjectManager **managerPtr;
    struct SceneActorRouteCursor *route;
    u8 step;
    u32 *np;
    struct NamedObjectManager *m;
    if (o->stage) {
        struct NamedObjectManager *manager = gNamedActorManager;
        if (HasFlag(0x2000, &o->flags) || o->name == manager->followName) {
            if (o->script) {
                struct NamedObjectArgs args;
                u32 values[3];
                if (o->kind == 2) {
                    if (HasFlag(2, &o->flags))
                        values[0] = 2;
                    else
                        values[0] = 1;
                } else
                    values[0] = o->route.pointIndex;
                values[1] = Load(&o->body.position.x);
                values[2] = Load(&o->body.position.z);
                args.count = 3;
                args.values = values;
                Script_Call(o->script, &args);
            }
        }
        o->timer = 0;
        o->stage = 0;
    }
    managerPtr = &gNamedActorManager;
    if (!sub_08101094(o)) {
        route = &o->route;
        step = route->unknown3++;
        if (step >= Route_GetCurrentPointNodeId(route)) {
            Route_AdvancePoint(route);
            if ((u8)SceneActorRoute_IsOutsidePointBounds(route, &o->body.position, 16, 16)) {
                o->state = 1;
                m = *managerPtr;
                np = &o->name;
                if (*np == m->followName && o->exitScript) {
                    struct NamedObjectArgs args;
                    u32 values[1];
                    values[0] = *np;
                    args.count = 1;
                    args.values = values;
                    Script_Call(o->exitScript, &args);
                }
            } else
                o->state = 3;
        }
    }
}
