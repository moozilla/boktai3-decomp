#include "global.h"

#ifdef NONMATCHING
// fuzzy: 82.5%
// fix.py best version (reprinted C)
// Remaining differences: coordinate lifetimes, stack spills, shared branch tails and registers.
struct Part {
    u8 pad[0x38];
    u32 flags;
};
struct Target {
    u8 pad[0x30];
    s16 x, y, z;
};
struct Armor {
    u8 pad0[0x48];
    s16 x, y, z;
    u8 pad1[0x190 - 0x4e];
    struct Part body;
    u8 pad2[0x1e4 - 0x1cc];
    struct Part attackCollider;
    u8 pad3[0xb26 - 0x220];
    s16 attack;
    u8 pad4[0xb32 - 0xb28];
    u16 flags;
    u8 pad5[0xb38 - 0xb34];
    u8 type, facing, b3a, b3b, behavior, phase;
    u8 pad6[0x1120 - 0xb3e];
    u8 mode, boost;
    s16 timer, delay, accumulated, contacts, boostTimer, initialAttack;
};
extern struct Target *gGameState;
void ActorController_SetBehavior(struct Armor *, s32);
s32 Math_Atan2(s32, s32);
static inline void clear(struct Part *c, u32 bits)
{
    c->flags &= ~bits;
}
// confidence=low: close-range pursuit and boosted attack choices, gated by actor flags.
void Armor_UpdateFacingDecision(struct Armor *p)
{
    s32 tx;
    s32 tz;
    s32 x;
    s32 z;
    s32 dx;
    s32 dz;
    s32 ax;
    s32 az;
    s32 dist;
    s32 dir;
    u32 flag;
    u32 k1 = 192;
    if (p->timer > 0)
        p->timer--;
    if (p->boostTimer > 0) {
        p->boostTimer--;
        if (p->boostTimer <= 0) {
            p->mode = 4;
            p->boost = 0;
            p->timer = 300;
            p->attack = p->initialAttack;
            p->contacts = 0;
            clear(&p->attackCollider, 0x400008);
            clear(&p->body, 0x400008);
        }
    }
    if (p->delay > 0)
        p->delay--;
    if (((((p->phase != 255) && (p->behavior != 2)) && (p->behavior != 38)) &&
         (p->behavior != 37)) &&
        (p->behavior != 39))
        return;
    tx = gGameState->x;
    x = p->x;
    dx = tx - x;
    tz = gGameState->z;
    z = p->z;
    dz = tz - z;
    dx += 128;
    dx >>= 8;
    dz += 128;
    dz >>= 8;
    ax = dx;
    az = dz;
    if (dx < 0)
        ax = -dx;
    if (dz < 0)
        az = -dz;
    flag = p->flags & 1;
    if (flag) {
        switch (p->mode) {
            case 0:
                if ((ax + az) <= 4) {
                    if (p->boost)
                        ActorController_SetBehavior(p, 39);
                    else
                        ActorController_SetBehavior(p, 37);
                    p->mode = 1;
                    p->timer = 360;
                } else {
                    if (p->boost || (p->delay <= 0))
                        goto approach;
                    goto idle;
                }
                break;

            case 1:
                if (p->behavior == 3)
                    goto pursue;
                dir = Math_Atan2(gGameState->x - p->x, gGameState->z - p->z);
                dir += 32;
                dir &= k1;
                if (p->facing == dir) {
                    dist = ax + az;
                    if (dist > 5) {
                        ActorController_SetBehavior(p, 9);
                        p->mode = 0;
                        break;
                    }
                    if (dist <= 1)
                        goto attack;
                }
                if ((p->timer <= 0) && ((dx == 0) || (dz == 0))) {
                    if (p->boost)
                        ActorController_SetBehavior(p, 36);
                    else
                        ActorController_SetBehavior(p, 24);
                    p->timer = 360;
                } else {
                    if (p->boost || (p->delay <= 0))
                        goto close;
                    goto hold;
                }
                break;

            case 2:
                ActorController_SetBehavior(p, 3);
                p->mode = 1;
                p->timer = 360;
                break;

            case 3:
                if ((((u8)(p->behavior - 10)) > 1) && (p->behavior != 13)) {
                    ActorController_SetBehavior(p, 11);
                    break;
                }
                if (p->timer <= 0) {
                    ActorController_SetBehavior(p, 12);
                    p->mode = 0;
                    p->accumulated = 0;
                } else if (p->boost) {
                    ActorController_SetBehavior(p, 12);
                    p->mode = 0;
                    p->accumulated = 0;
                }
                break;

            case 4:
                if (p->timer <= 0) {
                    ActorController_SetBehavior(p, 2);
                    p->mode = 0;
                    p->boost = 0;
                    p->contacts = 0;
                } else
                    ActorController_SetBehavior(p, 29);
                break;
        }

    } else {
        if (p->type == 15) {
            ActorController_SetBehavior(p, 41);
            return;
        }
        switch (p->mode) {
            case 0:
                if ((ax + az) <= 4) {
                    if (p->boost)
                        ActorController_SetBehavior(p, 39);
                    else
                        ActorController_SetBehavior(p, 37);
                    p->mode = 1;
                    p->timer = 360;
                } else if (p->boost || (p->delay <= 0)) {
                approach:
                    ActorController_SetBehavior(p, 38);

                } else {
                idle:
                    ActorController_SetBehavior(p, 2);
                }
                break;

            case 1:
                if (p->behavior == 3) {
                pursue:
                    ActorController_SetBehavior(p, 30);

                    break;
                }
                dir = Math_Atan2(tx - x, tz - z);
                dir += 32;
                dir &= k1;
                if (p->facing == dir) {
                    dist = ax + az;
                    if (dist > 5) {
                        ActorController_SetBehavior(p, 9);
                        p->mode = flag;
                        break;
                    }
                    if (dist <= 1) {
                    attack:
                        ActorController_SetBehavior(p, 3);

                        break;
                    }
                }
                if ((p->timer <= 0) && ((dx == 0) || (dz == 0))) {
                    if (p->boost)
                        ActorController_SetBehavior(p, 36);
                    else
                        ActorController_SetBehavior(p, 24);
                    p->timer = 360;
                } else if (p->boost || (p->delay <= 0)) {
                close:
                    ActorController_SetBehavior(p, 39);

                } else {
                hold:
                    ActorController_SetBehavior(p, 37);
                }
                break;

            case 2:
                ActorController_SetBehavior(p, 3);
                p->mode = 1;
                p->timer = 360;
                break;

            case 4:
                if (p->timer <= 0) {
                    ActorController_SetBehavior(p, 2);
                    p->mode = flag;
                    p->boost = flag;
                    p->contacts = flag;
                } else
                    ActorController_SetBehavior(p, 29);
                break;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Armor_UpdateFacingDecision);
#endif
