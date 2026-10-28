#include "named_object.h"
#include "player.h"
static inline u8 HasFlag(u32 mask, s16 *flags)
{
    if ((u16)*flags & mask)
        return TRUE;
    return FALSE;
}
static inline void ClearFlag(s32 mask, s16 *flags)
{
    *flags &= ~mask;
}
static inline void SetFlag2(u16 mask, u16 *flags)
{
    *flags = mask | *flags;
}
static inline s32 Load(s16 *p)
{
    return *p;
}
void NamedObject_UpdateContactTrigger(struct NamedObject *o)
{
    struct NamedObjectManager *manager;
    struct NamedObjectArgs args;
    u32 values[3];
    struct NamedObject *found;
    struct NamedObjectManager *m;
    u32 *np, name;
    if (o->stage) {
        manager = gNamedActorManager;
        if (HasFlag(0x2000, &o->flags) || o->name == manager->followName) {
            if (o->script) {
                if (o->kind == 2) {
                    if (HasFlag(2, &o->flags))
                        values[0] = 2;
                    else
                        values[0] = 1;
                } else
                    values[0] = o->route[2];
                values[1] = Load(&o->x);
                values[2] = Load(&o->z);
                args.count = 3;
                args.values = values;
                Script_Call(o->script, &args);
            }
        }
        o->stage = 0;
        o->timer = 1;
    }
    if (!sub_08101094(o)) {
        if (o->field_E0 & 1) {
            if (o->timer >= o->limit && NamedObject_IsPlayerWithinContactBounds(o)) {
                o->state = 2;
                if (HasFlag(2, &o->flags))
                    ClearFlag(2, &o->flags);
                else
                    SetFlag2(2, (u16 *)&o->flags);
                o->timer = 0;
                o->stage = 1;
                m = gNamedActorManager;
                np = &o->name;
                if (*np == m->followName && o->exitScript) {
                    struct NamedObjectArgs args2;
                    u32 values2[1];
                    values2[0] = *np;
                    args2.count = 1;
                    args2.values = values2;
                    Script_Call(o->exitScript, &args2);
                }
                return;
            }
            if (o->timer > o->limit - 10)
                SetFlag2(0x20, (u16 *)&o->flags);
        }
        found = NamedObject_FindNearPosition(((u8 **)gPlayers)[0] + 0x30);
        if (found) {
            if (found->name == o->name) {
                if (!HasFlag(0x1000, &o->flags) && o->timer == 0) {
                    o->offsetC2 = -0x20;
                    Sound_PlaySong(0x222);
                }
                if (o->timer <= 0x7CFF)
                    o->timer++;
            }
        } else {
            o->timer = 0;
            o->offsetC2 = 0;
        }
    }
}
