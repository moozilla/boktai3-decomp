#include "global.h"
#include "scene_actor_reset.h"
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
s32 Div(s32, s32);
static inline u32 Flag(struct SceneActorRegistryResetView *a, u32 m, u32 off)
{
    return off == 0x2D4 ? *(u16 *)((u8 *)a + off) & m : *(u32 *)((u8 *)a + off) & m;
}
static inline u8 Unavailable(struct SceneActorRegistryResetView *a)
{
    if (a && (Flag(a, 0x40, 0x2D4) || Flag(a, 0x20000, 0x2D0) || a->health <= 0))
        return 1;
    return 0;
}
void Golem_UpdateAttackProperties(struct SceneActorRegistryResetView *a)
{
    u8 *state = a->context;
    u8 *group = (u8 *)&a->group.common;
    if (Unavailable(a))
        return;
    if (Flag(a, 0xA027, 0x2D4))
        return;
    if (Flag(a, 0x10, 0x2D4)) {
        struct ContactCollider *c = (void *)((u8 *)a + 0x188);
        c->contactFlags = 0x20;
        c->flags = 0x2001;
        {
            u16 power = a->attacks.first[2];
            u16 response = a->attacks.second[2];
            u32 reaction = a->attacks.flags[2] | 0x200004;
            u16 value = a->attacks.parameter[2];
            Collider_SetAttackProperties(c, power, response, 0, reaction, value);
        }
        goto active;
    } else if (Flag(a, 0x100, 0x2D4)) {
        s32 speed = *(s16 *)(state + 0x682);
        s32 scale;
        s32 power, response;
        struct ContactCollider *c;
        if (speed >= 0)
            speed >>= 5;
        else
            speed = -(-speed >> 5);
        scale = speed;
        if (scale > 20)
            scale = 20;
        else if (scale < 10)
            scale = 10;
        power = Div(a->attacks.first[1] * scale, 10);
        response = Div(a->attacks.second[1] * (scale >> 1), 10);
        c = (void *)((u8 *)a + 0x188);
        {
            u32 flags = 0x2301;
            c->flags = flags;
            c->contactFlags = 0;
        }
        {
            u32 reaction = a->attacks.flags[1] | 0x40000;
            u16 value = a->attacks.parameter[1];
            Collider_SetAttackProperties(c, power, response, 0x400, reaction, value);
        }
        {
            struct Group {
                u32 a, flags;
            };
            struct Group *g = (void *)(group + 0x30);
            {
                u32 mask = 1;
                g->flags = mask | g->flags;
            }
        }
    active: {
        u32 mask = 0x4000;
        a->flags2D6 = mask | a->flags2D6;
    }
    } else {
        struct ContactCollider *c = (void *)((u8 *)a + 0x188);
        c->flags = 0x2001;
        c->contactFlags = 0x10;
        {
            u16 power = a->attacks.first[0];
            u16 response = a->attacks.second[0];
            u32 reaction = a->attacks.flags[0];
            u16 value = a->attacks.parameter[0];
            Collider_SetAttackProperties(c, power, response, 0, reaction, value);
        }
        return;
    }
}
