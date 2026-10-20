#include "global.h"
struct ProjectileTask {
    u8 pad0[0x1C];
    u32 ownerCounts[4];
    u32 totals[4];
    s32 limit;
};
struct ProjectileDef {
    u16 flags;
    u8 pad2[4];
    u8 angle;
    u8 pad7[0x38 - 7];
    s16 owner;
    u8 pad3A[0x48 - 0x3A];
    s32 kind;
};
void *ResourceRegistry_GetResource(u32);
void *Task4E01_GetOrCreate(void);
s32 Mod(s32, s32);
s32 TimedEffect_SpawnProjectile(void *, void *, void *);
s32 Projectile_Spawn(struct ProjectileDef *p, void *arg)
{
    s32 total = 0;
    struct ProjectileTask *task;
    s32 i;
    s32 step;
    s32 kind;
    s32 result;
    task = ResourceRegistry_GetResource(0x4E01);
    if (!task) {
        Task4E01_GetOrCreate();
        return -1;
    }
    {
        s32 limit = task->limit;
        u32 *t = task->totals;
        i = 3;
        do {
            total += *t++;
            i--;
        } while (i >= 0);
        if (total >= limit)
            return -1;
    }
    {
        u32 f = p->flags & 0x2010;
        kind = p->kind;
        if (!f) {
            if (kind == 1) {
                if (task->ownerCounts[p->owner] > 4)
                    return -1;
            } else if (kind == 0 || kind == 4 || kind == 3 || kind == 5 || kind == 6) {
                if (task->ownerCounts[p->owner] > 2)
                    return -1;
            } else {
                if (task->ownerCounts[p->owner] != 0)
                    return -1;
            }
        }
    }
    if (kind == 2) {
        step = 0x10;
        p->angle -= 0x20;
        for (i = 0; i <= 4; i++) {
            p->flags &= 0xF0FF;
            if (Mod(i, 2) == 0)
                p->flags |= 0x100;
            else
                p->flags |= 0x200;
            result = TimedEffect_SpawnProjectile(task, p, arg);
            p->angle += step;
            if (result == -1)
                return -1;
        }
        return result;
    }
    return TimedEffect_SpawnProjectile(task, p, arg);
}
