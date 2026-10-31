#include "global.h"
#include "engine.h"
#include "ram.h"
#include "scene_actor_chase_shared.h"
#include "scene_actor_control_phase.h"
#include "player.h"
#include "contact_swarm.h"
struct AxeContext {
    u8 pad0[0xA38];
    u32 spriteFlags;
    u8 padA3C[0x2];
    u8 spriteAngle;
    u8 padA3F[0x15];
    u16 spriteX;
    u8 padA56[0x2];
    u16 spriteZ;
    u8 padA5A[0x36];
    struct ContactCollider attack[3];
    struct ContactCollider body[3];
    struct ContactPosition origin;
    u8 padC90[0x8];
    struct ContactPosition center;
    s16 radius;
    u8 padCA2[0x2];
    u8 speed;
    s8 direction;
    u8 heading;
    u8 padCA7[0x2];
    u8 angle;
    u8 padCAA[0x1];
    u8 flags;
    u8 padCAC[0x204];
    u8 timer;
};
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
void Axe_UpdateAttackColliderPositions(void *);
void ActorPhase_UpdateAcceleratingTrail(void);
void ActorPhase_UpdateReverseFlicker(void);
static inline u8 Axe_TakeEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline void Axe_Difference(struct ContactPosition *v, struct ContactPosition *b,
                                  struct ContactPosition *a)
{
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}
static inline void Axe_AddPositions(struct ContactPosition *out, struct ContactPosition *a,
                                    struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
static inline s32 Axe_ScaleTowardZero(s32 value, s32 bits)
{
    if (value >= 0)
        return value >> bits;
    return -(-value >> bits);
}
static inline s32 Axe_RandomByte(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u8 *)&((const u16 *)RANDOM_TABLE_ADDR)[index];
}

static inline s32 Axe_ReadContactIndex(const s32 *index)
{
    return *index;
}
static inline s32 Axe_ContactIndex(s32 index)
{
    return Axe_ReadContactIndex(&index);
}
/* Orbit the actor and its auxiliary sprite around a moving shared center. */
void Axe_UpdateOrbitAttack(struct SceneActorRegistryResetView *p, s32 frame)
{
    struct AxeContext *data = p->context;
    struct ContactPosition delta;
    struct ContactPosition *position;
    u8 entered;
    s32 difference, absolute;
    entered = Axe_TakeEntry(&p->field2B2);
    position = &p->body.position;
    if (entered) {
        data->flags |= 2;
        data->angle = 0;
        data->radius = 128;
        data->speed = 4;
        data->direction = 1;
        data->center = data->origin;
        Axe_Difference(&delta, &data->center, position);
        delta.x = Axe_ScaleTowardZero(delta.x, 1);
        delta.z = Axe_ScaleTowardZero(delta.z, 1);
        Axe_AddPositions(&data->center, &data->center, &delta);
    }
    position->x = data->center.x +
                  Axe_ScaleTowardZero(gUnk_086149C4[(data->angle + 64) & 255] * data->radius, 12);
    p->body.position.z =
        data->center.z + Axe_ScaleTowardZero(gUnk_086149C4[data->angle] * data->radius, 12);
    data->spriteX = data->center.x +
                    Axe_ScaleTowardZero(
                        gUnk_086149C4[(((data->angle + 128) & 255) + 64) & 255] * data->radius, 12);
    data->spriteZ =
        data->center.z +
        Axe_ScaleTowardZero(gUnk_086149C4[(data->angle + 128) & 255] * data->radius, 12);
    if (frame < 60) {
        data->speed += 2;
        data->timer = 0;
    } else if (frame < 180) {
        if (Mod(frame, 4) == 0) {
            if (Axe_RandomByte() <= 19) {
                {
                    s32 direction = data->direction;
                    data->direction = -direction;
                }
                Sound_PlaySong(0x169);
            }
        }
        Axe_Difference(&delta, &data->center, (void *)&gPlayers[0]->position);
        data->heading = Math_Atan2(delta.x, delta.z);
        data->timer = 0;
    } else {
        if (!(data->flags & 4)) {
            if (data->radius < 512) {
                data->radius += 8;
                data->speed -= 4;
                if (data->speed <= 15)
                    data->speed = 16;
            } else {
                data->radius = 512;
                data->speed = 16;
                data->flags |= 4;
            }
        }
        Axe_Difference(&delta, &data->center, (void *)&gPlayers[0]->position);
        difference = Math_Atan2(delta.x, delta.z) - data->heading;
        if (difference > 128)
            difference -= 256;
        else if (difference < -128)
            difference += 256;
        absolute = difference;
        if (difference < 0)
            absolute = -difference;
        if (absolute > 20) {
            if (difference > 0)
                data->heading += 4;
            else
                data->heading -= 4;
        }
        data->center.x += Axe_ScaleTowardZero(gUnk_086149C4[(data->heading + 64) & 255] * 2, 12);
        data->center.z += Axe_ScaleTowardZero(gUnk_086149C4[data->heading] * 2, 12);
        if (data->flags & 4) {
            struct DisplaySequenceSprite *sprite = p->drawable;
            u32 previous;
            sprite->flags |= 2;
            data->spriteFlags |= 2;
            previous = sprite->attribute;
            sprite->attribute = previous - 6;
            data->spriteAngle += 6;
            if ((s32)previous < ((struct DisplaySequenceSprite *)p->drawable)->attribute)
                Sound_PlaySong(0x169);
            if (data->flags & 4) {
                if (frame < 480)
                    data->radius--;
                else if (frame < 580)
                    data->radius += 6;
                else if (frame < 660)
                    data->radius -= 4;
                else if (frame < 740)
                    data->radius += 4;
                else if (frame > 819)
                    data->radius -= 6;
            }
        }
    }
    data->angle += Axe_ScaleTowardZero(data->speed * (s8)data->direction, 4);
    Axe_UpdateAttackColliderPositions(p);
    if (data->flags & 1) {
        SceneActorChase_SetControl(p, ActorPhase_UpdateAcceleratingTrail, 0x2E);
    } else if (frame > 1000) {
        s32 i;
        u32 stride = sizeof(struct ContactCollider);
        {
            struct ContactCollider *c = &p->colliderB;
            c->flags |= 4;
        }
        {
            struct ContactCollider *c = &p->colliderA;
            c->flags |= 4;
        }
        i = 0;
        do {
            u32 attackStride = sizeof(data->attack[0]);
            struct ContactCollider *attackBase = data->attack;
            struct ContactCollider *attack = (void *)((u8 *)attackBase + i * attackStride);
            typedef u8 AxeBodyRecord[stride];
            struct ContactCollider *body =
                (struct ContactCollider *)&((AxeBodyRecord *)data->body)[i];
            attack->flags |= 4;
            body->flags |= 4;
            ++i;
        } while (Axe_ContactIndex(i) <= 2);
        SceneActorChase_SetControl(p, ActorPhase_UpdateReverseFlicker, 0x2F);
    }
}
