#include "player_cue_sequence.h"
#include "contact_swarm.h"
#include "engine.h"
#include "ram.h"
struct Slash {
    u32 flags;
    u8 active, unknown5, phase, priority;
    s8 scaleX, scaleY;
    u8 unknownA[2];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    u8 trails[0xB8];
    u8 primary[0x54], secondary[0x54];
    struct ContactPosition velocity;
    u8 initialHeading, heading, turnTimer, turnSpeed;
    u16 speed;
    u8 trailIndex, trailPeriod;
    const void *sequenceResource;
    s32 lifetime;
    u16 effectFlags;
    u8 state;
    s8 slot;
    u16 timer;
    u8 entered, turnDelay;
};
struct Pool {
    u8 task[0x18];
    u8 graphics[0x20];
    u32 used;
};
extern const s16 gUnk_086149C4[];
struct Pool *SlashEffect_GetOrCreateManager(void);
struct Slash *SlashEffect_AllocateSlot(struct Pool *);
void TwinSpriteEffect_InitGraphics(struct Slash *);
void PursuingActor_InitColliders(struct Slash *, s32, s32, s32, s32, s32, s32, s32);
void TimedEffect_UpdateVisibility(struct Slash *);
void TwinSpriteEffect_EmitTrail(struct Slash *);
void PursuingActor_TurnTowardPlayer(struct Slash *);
void SlashEffect_UpdateColliders(struct Slash *);
void SwooshProjectile_RemoveTrailSprites(struct Slash *);
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
// confidence=low: paired-collider slash resource 8DC7, with a homing heading and trail sprites
void PursuingActor_CreateHomingSlash(const struct ContactPosition *position, s32 heading,
                                     s32 turnTimer, s32 turnSpeed, s32 turnDelay, s32 speed,
                                     s32 phase, s32 a, s32 b, s32 c, s32 d)
{
    struct Pool *pool;
    struct Slash *slot;
    struct Slash *sprite;
    pool = gPursuingActorManager;
    if (!pool) {
        pool = SlashEffect_GetOrCreateManager();
        if (!pool)
            return;
    }
    slot = SlashEffect_AllocateSlot(pool);
    if (!slot)
        return;
    slot->state = 1;
    slot->lifetime = 120;
    slot->effectFlags = 0;
    slot->initialHeading = heading;
    slot->heading = heading;
    slot->turnTimer = turnTimer;
    slot->turnSpeed = turnSpeed;
    slot->turnDelay = turnDelay;
    slot->speed = speed;
    slot->entered = 1;
    sprite = slot;
    slot->position = *position;
    slot->sequenceResource = ScriptProc_Start(0x922E, 0x8DC7);
    Sprite_Init(slot, pool->graphics, 0);
    Anim_SetSequence(&slot->sequence, (void *)slot->sequenceResource, 1, 0, 0);
    AnimData_SelectPaletteBank(slot->graphics, 0x2E2);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)slot, &slot->sequence);
    sprite->priority = 1;
    sprite->flags = (sprite->flags | 2) & ~1;
    sprite->phase = phase;
    TwinSpriteEffect_InitGraphics(slot);
    PursuingActor_InitColliders(slot, a, b, c, d, 0, 0, 0);
}
