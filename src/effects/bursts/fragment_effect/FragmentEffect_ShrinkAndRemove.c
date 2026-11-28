#include "effects/bursts/fragment_effect.h"
#include "player/cutscene/player_cue_sequence.h"

s32 Mod(s32, s32);
s32 Div(s32, s32);
void FragmentEffect_SpawnRandomParticle(struct ContactPosition *);

// Tentative: shrink the fragment, then release its sprite and pool slot.
void FragmentEffect_ShrinkAndRemove(struct FragmentEffectPool *pool, struct Fragment *fragment)
{
    if (fragment->entered) {
        fragment->sprite.flags = (fragment->sprite.flags & ~1) | 2;
        fragment->scaleTimer = 20;
        fragment->timer = 0;
        fragment->entered = 0;
        fragment->sprite.scaleX = 64;
        fragment->sprite.scaleY = 64;
        fragment->timer = 0;
    }
    if (!Mod(fragment->timer, 6))
        FragmentEffect_SpawnRandomParticle(&fragment->sprite.position);
    if (fragment->scaleTimer > 0) {
        fragment->sprite.scaleX += Div(1 - fragment->sprite.scaleX, fragment->scaleTimer);
        fragment->sprite.scaleY += Div(1 - fragment->sprite.scaleY, fragment->scaleTimer);
        fragment->scaleTimer--;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&fragment->sprite, &fragment->sequence);
    if (fragment->scaleTimer <= 0) {
        Sprite_Remove(&fragment->sprite);
        pool->occupied &= ~(1 << fragment->slot);
        fragment->slot = -1;
        fragment->state = 0;
        fragment->entered = 0;
    } else {
        fragment->timer++;
    }
}

struct FragmentEffectPool *FragmentEffect_GetOrCreateManager(void);
struct Fragment *sub_0812C3D0(struct FragmentEffectPool *);
void FragmentEffect_InitCollider(struct Fragment *, u32, u32, u32, u32, u32, u32, u32);

void FragmentEffect_Create(const struct ContactPosition *position, u32 angle, u32 angularSpeed,
                           s32 radius, s32 direction, s32 lifetime, u32 a, u32 b, u32 c, u32 d)
{
    struct FragmentEffectPool *pool = (struct FragmentEffectPool *)gFragmentEffectManager;
    struct Fragment *fragment;
    struct FragmentEffectSprite *sprite;
    struct ContactPosition offset;
    u32 heading;

    if (!pool) {
        pool = FragmentEffect_GetOrCreateManager();
        if (!pool)
            return;
    }
    fragment = sub_0812C3D0(pool);
    if (!fragment)
        return;
    fragment->state = 1;
    fragment->lifetime = lifetime;
    fragment->unk_B8 = 0;
    fragment->angle = angle << 12;
    fragment->angularSpeed = angularSpeed;
    fragment->radius = radius;
    fragment->direction = direction;
    fragment->entered = 1;
    fragment->center = *position;
    sprite = &fragment->sprite;
    sprite->position = fragment->center;
    heading = fragment->angle;
    FragmentEffect_ComputeOffset(&offset, heading, fragment->radius);
    sprite->position.x += offset.x;
    sprite->position.z += offset.z;
    fragment->sequenceResource = ScriptProc_Start(0x922e, 0x121b);
    Sprite_Init(sprite, pool->graphics, 0);
    Anim_SetSequence(&fragment->sequence, fragment->sequenceResource, 7, 0, 0);
    AnimData_SelectPaletteBank(sprite->graphics, 0x127);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&fragment->sprite, &fragment->sequence);
    sprite->priority = 2;
    sprite->flags &= ~1;
    FragmentEffect_InitCollider(fragment, a, b, c, d, 0, 0, 0);
}
