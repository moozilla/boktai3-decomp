#include "global.h"
#include "engine.h"
#include "player.h"
#include "ram.h"

struct RatatoskOrbitPoint {
    u32 xy, z;
};
struct RatatoskOrbitActor {
    u8 pad0[0x54];
    struct SceneVec position;
    u8 pad5c[0x34];
    u32 timer;
    u8 pad94[0x16];
    u8 animation, padAB[4], ready, active;
    u8 padB1[0x1b];
    u8 angle;
    u8 padCD[0xb3];
    s32 sequence, complete;
    u8 pad188[0xb44];
    struct RatatoskOrbitPoint points[9];
    u8 padD14[0x146];
    u16 repeat;
    struct SceneVec destination, center;
    u16 duration;
    u8 padE6e[6];
    s16 hitTimer;
    u8 padE76[0x14];
    s16 previousAngle, ended;
    u8 padE8e[0x50];
    s16 waitTimer, particleTimer;
    u8 padEe2[4];
    s16 angleLimit;
};
extern const s16 gUnk_086149C4[];
struct SceneVec *Player_GetEffectivePosition(s32);
s32 Div(s32, s32);
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
void CpuSet(const void *, void *, u32);
void Player_BeginInterpolatedMovement(void *, void *, s32, s32, s32, s32, s32);
void Player_ApplyInterpolatedMovement(void *, void *, s32, s32, s32, s32, s32, s32);
void Player_EndState25(void *);
void SlotMotion_Initialize(void *);
void Ratatosk_ConfigureParticle(void *, s32, s32, s32, s32, s32);
void SceneParticles_SpawnAtPosition(void *, void *);
static inline void setAnimation(struct RatatoskOrbitActor *p, u32 animation)
{
    p->animation = animation;
}
static inline u8 consume(struct RatatoskOrbitActor *p)
{
    if (p->active) {
        p->active = 0;
        p->ready = 0;
        return 1;
    }
    return 0;
}
static inline u32 randomB(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
}
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct SceneVec *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}
static inline void difference(struct SceneVec *d, struct SceneVec *a, struct SceneVec *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
static inline u32 flag(struct Player *p, u32 mask)
{
    return p->statusFlags & mask;
}
static inline void ClearVector(struct SceneVec *vector, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, vector, 0x05000002);
}
// confidence=low: draws the player into a midpoint orbit before releasing movement.
#define Ratatosk_UpdatePlayerOrbitAttack sub_081F7FD0
inline static void sum_081F7FD0(struct SceneVec *d, struct SceneVec *a, struct SceneVec *b)
{
    d->x = b->x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}

void Ratatosk_UpdatePlayerOrbitAttack(struct RatatoskOrbitActor *p)
{
    struct RatatoskOrbitActor *q = p;
    struct {
        u32 zero;
        u32 config[6];
    } scratch;
    struct SceneVec offsetVector, deltaVector, positionVector;
    if (consume(q)) {
        setAnimation(q, 9);
        p->sequence = 0;
        p->repeat = 0;
        p->waitTimer = 0;
        p->duration = 64;
        Sound_PlaySong(0x558);
    }
    switch (p->sequence) {
        case 0:
            if (q->ready) {
                if (p->hitTimer > 0) {
                    struct SceneVec *playerPosition;
                    struct Player *player;
                    p->sequence = 1;
                    playerPosition = Player_GetEffectivePosition(0);
                    difference(&p->center, playerPosition, &q->position);
                    p->center.x = q->position.x + Div(p->center.x, 2);
                    p->center.y = q->position.y;
                    p->center.z = q->position.z + Div(p->center.z, 2);
                    p->duration = 128;
                    polar(&offsetVector, ((((((u8)randomB()) + 0x60) >> 6) & 3) * 64) + 64, 512);
                    sum_081F7FD0(&p->destination, &offsetVector, &p->center);
                    player = gPlayers[0];
                    if (flag(player, 0x40000) != 1) {
                        Player_BeginInterpolatedMovement(player, &p->center, p->duration, 0, 476, 0,
                                                         0);
                        p->previousAngle = gPlayers[0]->facingChangeEffects;
                        p->ended = 0;
                        Sound_PlaySong(0x569);
                    }
                    setAnimation(q, 5);
                } else if ((++p->waitTimer) > 2) {
                    p->duration = 64;
                    p->complete = 1;
                } else {
                    s32 index = Mod(randomB(), 9);
                    struct RatatoskOrbitPoint *destination =
                        (struct RatatoskOrbitPoint *)&p->destination;
                    u32 address = index * sizeof(struct RatatoskOrbitPoint) + (u32)p;
                    address += (u32) & ((struct RatatoskOrbitActor *)0)->points;
                    *destination = *(struct RatatoskOrbitPoint *)address;
                    setAnimation(q, 4);
                    p->sequence = 3;
                }
            }
            break;

        case 1: {
            s32 prev = p->previousAngle;
            struct Player *player = gPlayers[0];
            s32 amount = player->facingChangeEffects;
            if (amount < prev) {
                s32 wrapped = prev - 256;
                amount -= wrapped;
            } else
                amount -= prev;
            if (amount > p->angleLimit) {
                if (p->ended)
                    goto finish;
                Player_EndState25(gPlayers[0]);
                p->ended = 1;
                Sound_StopSong(0x569);
            }
            if (!p->ended) {
                {
                    s32 sample = (u8)randomB();
                    if (sample < 8 && !p->particleTimer)
                        p->particleTimer = 10;
                }
                if (p->particleTimer > 0) {
                    s32 step = Mod(q->timer, 32);
                    struct SceneVec *delta;
                    struct SceneVec *position;
                    struct SceneVec *effectivePosition;
                    difference(&deltaVector, &q->position, Player_GetEffectivePosition(0));
                    delta = &deltaVector;
                    if (!step) {
                        ClearVector(delta, &scratch.zero);
                    } else {
                        delta->x = Div(delta->x * step, 32);
                        delta->y = 196;
                        delta->z = Div(delta->z * step, 32);
                    }
                    effectivePosition = Player_GetEffectivePosition(0);
                    position = &positionVector;
                    sum_081F7FD0(position, &deltaVector, effectivePosition);
                    position->y += 128;
                    SlotMotion_Initialize(scratch.config);
                    Ratatosk_ConfigureParticle(scratch.config, 10, 0, 0, 0, 0);
                    SceneParticles_SpawnAtPosition(scratch.config, position);
                    p->particleTimer--;
                }
            }
        finish:
            if (q->ready) {
                setAnimation(q, 7);
                p->waitTimer = 0;
                p->sequence = 2;
            }

            break;
        }

        case 2:
            if ((p->waitTimer == 13) && (!p->ended)) {
                struct Player *player = gPlayers[0];
                if (flag(player, 0x40000) != 1)
                    Player_ApplyInterpolatedMovement(player, &p->center, 30, q->angle, 0, 42, 30,
                                                     0);
                Sound_StopSong(0x569);
            }
            if (q->ready) {
                p->duration = 64;
                p->complete = 1;
            }
            p->waitTimer++;
            break;

        case 3:
            if (q->ready) {
                setAnimation(q, 9);
                p->sequence = 0;
                p->repeat = 0;
            }
            break;
    }
}
