#include "global.h"

void Sound_PlaySong(u32);

s32 Collider_Refresh(u8 *);

void Task_Kill(u8 *);

static inline void frame(u8 *p)
{
    *(u16 *)(p + 0x28) = *(u16 *)(p + 0xb6) >> 2;
}

s32 Explosion_Update(u8 *p)
{
    u16 *delay = (u16 *)(p + 0xb4);
    if (*delay)
        (*delay)--;
    else {
        u16 *age;
        u32 value;
        frame(p);
        value = *(u16 *)(p + 0xb6);
        age = (u16 *)(p + 0xb6);
        if (value == 0) {
            *(u32 *)(p + 0x18) &= ~1;
            if (*(s16 *)(p + 0xba) == 0)
                Sound_PlaySong(0x195);
        }
        if (*age <= 7 && *(s16 *)(p + 0xb8) == 0)
            Collider_Refresh(p + 0x60);
        (*age)++;
        if (*age > 23)
            Task_Kill(p);
    }
    return 0;
}

void Sprite_Remove(u8 *);

u32 Explosion_Destroy(u8 *p)
{
    Sprite_Remove(p + 0x18);
    return 0;
}

struct Pair {
    u32 a, b;
};

void AnimData_Load(u8 *, u32);

void Sprite_Init(u8 *, u8 *, u32);

void AnimData_SelectPaletteBank(u8 *, u32);

static inline void setFields(u8 *p, u32 bit)
{
    p[7] = bit;
    *(u32 *)p |= bit;
}

void Explosion_InitSprite(u8 *sprite, u8 *resource, struct Pair *pos, s32 frame)
{
    AnimData_Load(resource, 0x8639);
    Sprite_Init(sprite, resource, 0);
    *(u16 *)(sprite + 0x10) = 0;
    frame += 0x116;
    AnimData_SelectPaletteBank(resource, frame);
    *(struct Pair *)(sprite + 0x1c) = *pos;
    setFields(sprite, 1);
}

struct ExplosionSize {
    u32 x : 16, y : 16, z : 16;
};

void Collider_Init(u8 *, u32, u32, u32, u32, void *, void *);

void Collider_SetAttackProperties(u8 *, s32, s32, s32, s32, s32);

void Collider_SetCallback(u8 *, u32, u8 *);

void Explosion_InitCollider(u8 *obj, u8 *collider, u32 flag, s32 a3, s32 a4, s32 a5, s32 a6)
{
    struct ExplosionSize size, offset;

    size.x = 120;
    size.y = 120;
    size.z = 120;
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x2001, 0, (u16)flag, &size, &offset);
    Collider_SetAttackProperties(collider, a3, a4, 0x210, a5, a6);
    Collider_SetCallback(collider, 0, obj);
    if (a3 == 0 && a4 == 0)
        *(u16 *)(obj + 0xb8) = 1;
}

void Explosion_InitColliderWithFlags(u8 *obj, u8 *collider, u32 flag, s32 a3, s32 a4, s32 a5,
                                     s32 a6, s32 a7)
{
    struct ExplosionSize size, offset;

    size.x = 120;
    size.y = 120;
    size.z = 120;
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x2001, 0, (u16)flag, &size, &offset);
    Collider_SetAttackProperties(collider, a4, a5, 0x200 | a3, a6, a7);
    Collider_SetCallback(collider, 0, obj);
    if (a4 == 0 && a5 == 0)
        *(u16 *)(obj + 0xb8) = 1;
}

void Explosion_InitSprite(u8 *, u8 *, struct Pair *, s32);

void Explosion_InitCollider(u8 *, u8 *, u32, s32, s32, s32, s32);

void Collider_SetPosition(u8 *, u8 *, u32);

static inline void init(u16 *dest, u32 zero, u32 value)
{
    *dest++ = value;
    *dest = zero;
}

s32 Explosion_Init(u8 *p, u8 *pos, u32 flag, s32 duration, s32 arg4, s32 arg5, s32 arg6, s32 frame,
                   u32 arg8)
{
    u8 *emitter;
    Explosion_InitSprite(p + 0x18, p + 0x44, pos, frame);
    emitter = p + 0x60;
    Explosion_InitCollider(p, emitter, flag, duration, arg4, arg5 | 0x40000, arg6);
    Collider_SetPosition(emitter, pos, 0);
    init((u16 *)(p + 0xb4), 0, arg8);
    return 0;
}

void Explosion_InitColliderWithFlags(u8 *, u8 *, u32, s32, s32, s32, s32, s32);

s32 Explosion_InitWithCollisionFlags(u8 *p, u8 *pos, u32 flag, s32 duration, s32 arg4, s32 arg5,
                                     s32 arg6, s32 arg7, s32 frame, u32 arg9)
{
    u8 *emitter;
    Explosion_InitSprite(p + 0x18, p + 0x44, pos, frame);
    emitter = p + 0x60;
    Explosion_InitColliderWithFlags(p, emitter, flag, duration, arg4, arg5, arg6 | 0x40000, arg7);
    Collider_SetPosition(emitter, pos, 0);
    init((u16 *)(p + 0xb4), 0, arg9);
    return 0;
}

/* Explosion_CreateImmediate: Allocates an explosion with default collision parameters and zero activation delay */


u8 *Task_Create(u32, u32);

void Task_SetCallbacks(u8 *, void *, void *);

s32 Explosion_Init(u8 *, u8 *, u32, s32, s32, s32, s32, s32, u32);

u8 *Explosion_CreateImmediate(u8 *pos, u32 flag, s32 duration, s32 arg3, s32 arg4)
{
    u8 *obj = Task_Create(10, 188);
    if (obj) {
        Task_SetCallbacks(obj, Explosion_Update, Explosion_Destroy);
        if (Explosion_Init(obj, pos, flag, duration, arg3, 262144, 60, arg4, 0) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}

/* Explosion_CreateDelayed: Allocates an explosion with default collision parameters and a caller-supplied activation delay */


u8 *Explosion_CreateDelayed(u8 *pos, u32 flag, s32 duration, s32 arg3, s32 arg4, u32 arg5)
{
    u8 *obj = Task_Create(10, 188);
    if (obj) {
        Task_SetCallbacks(obj, Explosion_Update, Explosion_Destroy);
        if (Explosion_Init(obj, pos, flag, duration, arg3, 262144, 60, arg4, arg5) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}

u8 *Explosion_Create(u8 *pos, u32 flag, s32 duration, s32 arg3, s32 arg4, s32 arg5, s32 arg6,
                     u32 arg7)
{
    u8 *obj = Task_Create(10, 188);
    if (obj) {
        Task_SetCallbacks(obj, Explosion_Update, Explosion_Destroy);
        if (Explosion_Init(obj, pos, flag, duration, arg3, arg4, arg5, arg6, arg7) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}

u8 *Explosion_CreateAt(void *pos, u32 flag, s32 duration, s32 arg3, s32 arg4, s32 arg5, s32 frame,
                       u32 delay)
{
    u8 *obj = Task_Create(10, 188);
    if (obj) {
        Task_SetCallbacks(obj, Explosion_Update, Explosion_Destroy);
        if (Explosion_Init(obj, pos, flag, duration, arg3, arg4, arg5, frame, delay) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    *(u16 *)(obj + 0xba) = 1;
    return obj;
}

s32 Explosion_InitWithCollisionFlags(u8 *, u8 *, u32, s32, s32, s32, s32, s32, s32, u32);

u8 *Explosion_CreateWithCollisionFlags(u8 *pos, u32 flag, s32 duration, s32 arg3, s32 arg4,
                                       s32 arg5, s32 arg6, s32 arg7, u32 arg8)
{
    u8 *obj = Task_Create(10, 188);
    if (obj) {
        Task_SetCallbacks(obj, Explosion_Update, Explosion_Destroy);
        if (Explosion_InitWithCollisionFlags(obj, pos, flag, duration, arg3, arg4, arg5, arg6, arg7,
                                             arg8) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
