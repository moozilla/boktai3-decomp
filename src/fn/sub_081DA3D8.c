#include "contact_swarm.h"

struct TimedEffectConfig {
    u16 flags, strength, tracking;
    s8 direction;
    u8 turnSpeed;
    s32 speed;
    u32 unk_0C;
    void *callback;
    u8 unk_14[0x48 - 0x14];
    s32 type;
    u8 unk_4C[0x58 - 0x4C];
};
struct TimedEffectTail {
    s32 elapsed;
    s32 unk_04;
    s32 height;
    struct ContactPosition previous;
    u8 unk_14[0x3E - 0x14];
    s16 cooldown;
    u32 index;
};
struct TimedEffect {
    u32 frame, flags;
    u8 unk_08[8];
    struct ContactPosition velocity;
    struct TimedEffectConfig config;
    struct ContactSwarmGraphics graphics;
    struct ContactSwarmSprite sprite;
    struct ContactCollider colliders[2];
    void *body;
    s32 lifetime;
    struct TimedEffectTail tail;
};
struct TimedEffectManager {
    u8 unk_00[0x18];
    u32 activeMask;
    u8 unk_1C[0x3C - 0x1C];
    s32 capacity;
    u8 unk_40[0x108 - 0x40];
    struct TimedEffect *entries;
};
struct EffectPlayer {
    u8 unk_00[0x20];
    u32 flags;
};
extern struct EffectPlayer *gUnknown_02000580;
extern const s16 gUnk_086149C4[];
void TimedEffect_RemoveFromManager(struct TimedEffectManager *, struct TimedEffect *, s32);
s32 TimedEffect_UpdateTerrainTurn(struct TimedEffectManager *, struct TimedEffect *, s32,
                                  struct ContactPosition *);
void Task4E01_BeginImpactAnimation(struct TimedEffect *);

static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void Polar(struct ContactPosition *output, s32 direction, s32 speed)
{
    s32 x = Scale(gUnk_086149C4[(direction + 64) & 255] * speed);
    s32 zero = 0;
    output->x = x;
    output->y = zero;
    output->z = Scale(gUnk_086149C4[direction & 255] * speed);
}
static inline s32 TimedEffect_TurnSnapshot(struct TimedEffectManager *manager,
                                           struct TimedEffect *entry, s32 index,
                                           struct ContactPosition *velocity,
                                           struct ContactPosition **saved)
{
    s32 status;
    *saved = velocity;
    status = TimedEffect_UpdateTerrainTurn(manager, entry, index, velocity);
    return status;
}
#ifdef NONMATCHING
// fuzzy: 86.5%
// scope shape 16 (differences once low registers are set aside)
s32 TimedEffect_UpdateManager(struct TimedEffectManager *manager)
{
    struct TimedEffect *entry = manager->entries;
    struct TimedEffect *cursor;
    struct TimedEffectTail *tail;
    s32 i;
    i = 0;
    if (i < manager->capacity) {
        cursor = entry;
        tail = &entry->tail;
        do {
            if (manager->activeMask & (1 << i)) {
                struct ContactSwarmSprite *sprite = &entry->sprite;
                struct TimedEffectConfig *config = &entry->config;
                struct ContactCollider *collider;
                s32 status = sub_08249240(entry, (void *)i, cursor->config.callback);
                collider = &entry->colliders[0];
                if (status == 2) {
                    TimedEffect_RemoveFromManager(manager, entry, i);
                    goto next;
                }
                if (cursor->flags & 1) {
                    Polar(&entry->velocity, config->direction, config->speed);
                    {
                        s32 mask = ~1;
                        cursor->flags &= mask;
                    }
                }
                if (config->flags & 0x1000) {
                    struct ContactPosition *position = &sprite->position;
                    sprite->position.x += cursor->velocity.x;
                    position->y += cursor->velocity.y;
                    position->z += cursor->velocity.z;
                } else {
                    if (status != 1) {
                        struct ContactPosition *position = &sprite->position;
                        struct ContactPosition *rollbackPosition;
                        struct ContactPosition velocity;
                        struct ContactPosition *savedVelocity;
                        s32 type;

                        tail->previous = *position;
                        sprite->position.x += cursor->velocity.x;
                        position->y += cursor->velocity.y;
                        position->z += cursor->velocity.z;
                        type = config->type;
                        rollbackPosition = position;
                        if (type == 9 || type == 6)
                            sprite->position.y -= tail->elapsed;
                        velocity = cursor->velocity;
                        status =
                            TimedEffect_TurnSnapshot(manager, entry, i, &velocity, &savedVelocity);
                        if (status <= 0) {
                            if (status == -1) {
                                config->flags |= 0x20;
                                Task4E01_BeginImpactAnimation(entry);
                            } else if (config->type != 9) {
                                sprite->position.x -= velocity.x;
                                rollbackPosition->y -= savedVelocity->y;
                                rollbackPosition->z -= savedVelocity->z;
                            }
                        }
                    }
                    if (tail->cooldown > 0) {
                        {
                            s32 mask = ~2;
                            cursor->flags &= mask;
                        }
                        tail->cooldown--;
                    }
                    entry->colliders[1].flags |= 4;
                    if (config->type != 9) {
                        u32 mask = 2;
                        if (cursor->flags & mask) {
                            Collider_SetPosition(collider, &sprite->position, 0);
                            Collider_Refresh(collider);
                        }
                    }
                    {
                        struct EffectPlayer *player = gUnknown_02000580;
                        u32 flag = 0x80000;
                        player->flags |= flag;
                    }
                }
                cursor->frame++;
            }
        next:
            i++;
            cursor++;

            tail = (void *)((u8 *)tail + sizeof(*entry));
            entry++;
        } while (i < manager->capacity);
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", TimedEffect_UpdateManager);
#endif
