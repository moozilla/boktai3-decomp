#include "scene_actor_charge_offset.h"
#include "engine_globals.h"
#include "ram.h"
typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, enter;
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    u8 (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    s16 scale, minScale, maxScale, scaleStep;
    u8 pad34C[0x60];
    u32 mode;
} ActorAnimation;

static inline void setFacing(ActorAnimation *p, int angle)
{
    u32 value;
    int shifted;
    p->angle = angle;
    shifted = p->angle + 0x60;
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySongChecked(u32);
int Math_Atan2(int, int);
static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline int add(struct ContactPosition *a, struct ContactPosition *b)
{
    int zero;
    a->x += b->x;
    zero = 0;
    a->y += b->y;
    a->z += b->z;
    return zero;
}
void SceneActors_UpdateSwarmAttack(ActorAnimation *p, int tick)
{
    ActorAnimation *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    struct ContactPosition spawn;
    if (p->changed) {
        int sequence;
        p->changed = 0;
        sequence = choose(p->mode, 68, 30);
        {
            int offset = 0x240;
            SceneActors_StartDirectionalAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220, sequence, 2,
                                                  1, 4);
        }
        Sound_PlaySongChecked(860);
    }
    if (p->tryUpdate(p))
        return;
    if (p->reset(p))
        return;
    if (tick > 10 && (tick & 7) == 7) {
        int zero;
        int direction;
        if (p->facing == 0 || p->facing == 3)
            SceneActor_SetChargeOffset((p->facing * 64) - 64, &spawn, 180);
        else
            SceneActor_SetChargeOffset((p->facing * 64) - 64, &spawn, 110);
        zero = add(&spawn, (struct ContactPosition *)(position + 12));
        spawn.y += 300;
        direction = *((u8 *)p2 + 0x3ba);
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 random = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
            direction -= 8;
            random &= 15;
            direction += random;
        }
        ContactSwarm_SpawnCharge(*(void **)((u8 *)p2 + 0x9d4), &spawn, direction, 20, zero, zero,
                                 zero);
        Sound_PlaySongChecked(234);
    }
    if (*((u8 *)p2 + 0x25d) & 2) {
        if (tick > 40)
            p->finished = 1;
    }
}
