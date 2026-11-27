#include "scene/motion/route_sprite_playback.h"
#include "engine/video/display_animation.h"
void RouteSpriteActor_RunEventScript(struct RouteSpriteActor *);
void FragmentBurst_Create(struct ContactPosition *, u32, u32, u32, u32, u32, void *);
union EmissionPosition {
    struct ContactPosition position;
    struct ContactPackedVector packed;
};
static inline s32 RandomWith(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
static inline s32 Jitter(u32 center, u32 random)
{
    center -= 48;
    return center + (random & 63);
}
void RouteSpriteActor_TickEmitting(struct RouteSpritePool *pool, struct RouteSpriteActor *actor)
{
    struct ContactSwarmSprite *sprite = &actor->sprite;
    struct DisplaySequenceState *sequence = &actor->sequence;
    u8 *entered = &actor->entered;
    u32 ended;
    if (*entered) {
        Anim_SetSequence(sequence, pool->sequenceResource, 0xE, 0, 0);
        actor->frames = 0;
        *entered = 0;
    }
    actor->sprite.position = actor->savedPosition;
    if (actor->frames <= 9) {
        const u16 *table = gRandomTable;
        u32 random;
        random = RandomWith(table);
        actor->sprite.position.x = Jitter((u16)actor->savedPosition.x, random);
        {
            u16 sample = RandomWith(table);
            s16 center = actor->savedPosition.z;
            s16 result = Jitter((u16)center, sample);
            actor->sprite.position.z = result;
        }
    }
    if (actor->frames == 4) {
        union EmissionPosition position;
        AnimData_SelectPaletteBank(((struct DisplaySequenceSprite *)actor)->resource, 0x143);
        position.position = actor->sprite.position;
        position.packed.y -= 60;
        FragmentBurst_Create(&position.position, (u16)actor->amount, actor->kind,
                             actor->parameter, actor->option, 20, actor->context);
        RouteSpriteActor_RunEventScript(actor);
    }
    ended = PlayerCue_StepSequenceEnded(sprite, sequence);
    if (ended) {
        actor->cyclesRemaining--;
        AnimData_SelectPaletteBank(((struct DisplaySequenceSprite *)sprite)->resource, 0x140);
        if (actor->cyclesRemaining)
            actor->state = 2;
        else
            actor->state = 5;
        actor->entered = 1;
    } else {
        actor->frames++;
    }
}
