#include "global.h"
#include "player.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct Vec position, velocity;
    u8 pad64[0x2c];
    u32 timer;
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[2];
    u8 mode, padA3;
    u32 modeTime;
    u8 facing, facing8, padAA, animationId, state, nextState, active, finished, padB0, changed,
        padB2, locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x33];
    void (*update)(void);
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x67];
    u32 phase;
    u8 pad180[0x1ac];
    u8 animation[0x20];
    u8 animationData[0x20];
    struct Vec anchor;
    u8 pad374[0x58];
    struct Vec home;
    u8 pad3D4[0xa78];
    s16 selectedPoint, selectedTarget;
    u16 padE50;
    u16 scaleX, scaleZ, padE56, scaleStep;
    u8 padE5A[0x14];
    u16 motion;
    u8 motionFlag, padE71, targetAngle, phaseTimer;
    s16 hitTimer, terrainTimer;
};
#define H(o) (*(s16 *)((u8 *)p2 + (o)))
#define U(o) (*(u16 *)((u8 *)p2 + (o)))
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
void Sound_PlaySong(u32);
u8 Callback_InvokeActor(void *, void *);
static inline void facing(struct Actor *p, s32 angle)
{
    u32 v;
    s32 shift;
    p->angle = angle;
    shift = (p->angle + 0x60) >> 6;
    v = 3;
    v &= shift;
    p->facing = v;
    shift = (p->angle + 0x70) >> 5;
    v = 7;
    v &= shift;
    p->facing8 = v;
}
static inline void sum(struct Vec *d, struct Vec *a, struct Vec *b)
{
    d->x = b->x + a->x;
    d->y = a->y + b->y;
    d->z = b->z + a->z;
}
// confidence=low: fires a player-homing projectile at a facing-dependent offset.
void Projectile_InitParameters(void *);
void Projectile_SetLaunchParameters(void *, u16, u32, u8, u32);
void Projectile_SetHomingParameters(void *, s32, void *, s32);
void Projectile_SetSpriteParameters(void *, u32, u32, u32);
void Projectile_SetScriptProcedure(void *, u32, u16);
void Projectile_SetCollisionParameters(void *, s32, u32, s32, s32, s32);
void Projectile_SetBehaviorMode(void *, u16);
void Projectile_SpawnAtPosition(void *, struct Vec *);
void Ratatosk_UpdateHomingShot(struct Actor *p, s32 tick)
{
    u8 params[0x58];
    struct PackedVec offset;
    struct Vec spawn;
    struct Actor *p2 = p;
    if (p->changed) {
        p->changed = 0;
        facing(p, p->requestedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 13, 1, 1, 4);
    }
    if (tick == 28) {
        s32 direction = 0;
        switch (p->facing) {
            case 0:
                break;
            case 1:
                direction = 2;
                break;
            case 2:
                direction = 6;
                break;
        }
        Projectile_InitParameters(params);
        Projectile_SetLaunchParameters(params, 0, 180, (direction + 5) << 5, 24);
        Projectile_SetHomingParameters(params, 180, (*(u8 **)gPlayers) + 0x30, 4);
        Projectile_SetSpriteParameters(params, 0x8e5a, 0, 499);
        Projectile_SetScriptProcedure(params, 0x4363, 3);
        Projectile_SetCollisionParameters(params, 0, 0x80000, H(0xeae), H(0xeb0), H(0xeb2));
        Projectile_SetBehaviorMode(params, 0);
        U(0xefa)++;
        switch (p->facing) {
            case 0:
                offset.x = 96;
                offset.y = 0;
                offset.z = -184;
                break;
            case 1:
                offset.x = 112;
                offset.y = 0;
                offset.z = -28;
                break;
            case 2:
                offset.x = -28;
                offset.y = 0;
                offset.z = 112;
                break;
            case 3:
                offset.x = -184;
                offset.y = 0;
                offset.z = 96;
                break;
        }
        sum(&spawn, (struct Vec *)&offset, &p2->anchor);
        spawn.y += 150;
        Projectile_SpawnAtPosition(params, &spawn);
        Sound_PlaySong(0x56a);
    }
    {
        register s32 frame asm("r1") = tick;
        asm("" : "+r"(frame));
        if (frame > 30)
            p->finished = 1;
    }
    {
        register u32 offset asm("r2") = 0x104;
        asm("" : "+r"(offset));
        {
            register void **address asm("r0") = (void **)((u8 *)p + offset);
            {
                void *callback = *address;
                Callback_InvokeActor(p, callback);
            }
        }
    }
}
