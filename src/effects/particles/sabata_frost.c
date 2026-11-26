#include "effects/particles/sabata_frost.h"
#include "npc/otenko/scene_npc_transition_palette.h"
#include "engine/math/monster_particle_random.h"
#include "player/cutscene/player_cue_sequence.h"

struct SceneNpcTransitionActor *Actor_FindById(void *, s32);

/* Script: puts the scene NPC named -n into mode 0x1A (sets +0x21B, clears
   the state fields; tentative). */

void SceneNpc_SetSpecialModeFromScript(void)
{
    void *manager = gSceneActorListManager;
    struct SceneNpcTransitionActor *npc;
    if (manager && (npc = Actor_FindById(manager, Script_GetKeywordValue('n', 0))) != 0) {
        u8 mode;
        npc->scriptedMode = 1;
        mode = 0x1A;
        npc->phase = 0;
        npc->step = 0;
        npc->mode = mode;
        npc->modeTimer = 0;
        npc->changed = 1;
    }
}

extern const s16 gUnk_086149C4[];

static inline s32 ShiftRandom(u16 value, s32 shift)
{
    return value >> shift;
}

static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -(-n >> 12);
}

void SabataFrost_SpawnParticle(struct SabataFrost *input)
{
    struct SabataFrost *emitter = input;
    struct FrostParticle *slot = 0;
    s32 i;
    for (i = 0; i < 12; i++) {
        if (!emitter->particles[i].lifetime) {
            slot = &emitter->particles[i];
            break;
        }
    }
    if (slot) {
        u32 sampleOrigin;
        const u16 *table;
        u32 index;
        u32 secondIndex;
        s32 angle;
        s32 magnitude;
        s32 mask;
        s32 sample;
        struct ContactPosition *velocity;
        struct SabataFrost *animationOwner;
        slot->sprite.position = *emitter->source;
        PARTICLE_RANDOM_CAPTURE_ORIGIN(sampleOrigin);
        table = gRandomTable;
        index = (gRandomIndex + 1) & 1023;
        angle = ShiftRandom(table[index], 4);
        mask = 63;
        animationOwner = emitter;
        secondIndex = (index + 1) & 1023;
        gRandomIndex = secondIndex;
        sample = ShiftRandom(table[secondIndex], 4) & 1;
        velocity = &slot->velocity;
        angle &= mask;
        magnitude = sample + 1;
        velocity->x = Scale(gUnk_086149C4[(angle + 64) & 255] * magnitude);
        velocity->y = 0;
        velocity->z = Scale(gUnk_086149C4[angle] * magnitude);
        {
            s32 lift = ShiftRandom(
                ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR), 4);
            slot->velocity.y = (lift & 3) + 4;
        }
        slot->lifetime = 60;
        Anim_SetSequence(&slot->sequence, animationOwner->resource, 0, 0, 4);
        slot->sprite.flags &= ~1;
    }
}

// confidence=low: moves one frost particle, steps playback and hides it on expiry.

static inline void FrostParticle_AddVelocity(struct ContactPosition *out, struct ContactPosition *v)
{
    out->x = out->x + v->x;
    out->y = out->y + v->y;
    out->z = out->z + v->z;
}

void SabataFrost_UpdateParticle(struct FrostParticle *input)
{
    struct FrostParticle *particle = input;
    FrostParticle_AddVelocity(&particle->sprite.position, &particle->velocity);
    PlayerCue_StepSequence(&particle->sprite, &particle->sequence);
    if (!--particle->lifetime)
        particle->sprite.flags |= 1;
}

void SabataFrost_Step(struct SabataFrost *emitter)
{
    s32 idle = 0;
    s32 i;
    for (i = 0; i < 12; i++) {
        if (emitter->particles[i].lifetime != 0)
            SabataFrost_UpdateParticle(&emitter->particles[i]);
        else
            idle++;
    }
    if (emitter->emitting == 0) {
        if (idle == 12) {
            emitter->runtime.update = 0;
            emitter->runtime.age = 0;
        }
    } else if ((emitter->runtime.age & 3) == 0)
        SabataFrost_SpawnParticle(emitter);
    emitter->runtime.age++;
}

void SabataFrost_Update(struct SabataFrost *emitter)
{
    if (emitter->runtime.update)
        Callback_InvokeActor(emitter, emitter->runtime.update);
}

void SabataFrost_Destroy(struct SabataFrost *emitter)
{
    struct FrostParticle *particle = emitter->particles;
    s32 i;
    for (i = 11; i >= 0; i--) {
        Sprite_Remove(&particle->sprite);
        particle++;
    }
}

s32 SabataFrost_Initialize(struct SabataFrost *emitter)
{
    void *graphics = &emitter->graphics;
    void *spriteGraphics;
    struct FrostParticle *slot;
    u16 *lifetime;
    struct SabataFrostStatus *state;
    void (**update)(struct SabataFrost *);
    u32 *age;
    u32 zero;
    u32 one;
    s32 i;
    AnimData_Load(graphics, 0x4384);
    AnimData_SelectPaletteBank(graphics, 0xB6);
    emitter->resource = ScriptProc_Start(0x922E, 0xF62B);
    if (!emitter->resource)
        return -1;
    i = 0;
    spriteGraphics = graphics;
    state = &emitter->runtime;
    update = &state->update;
    age = &state->age;
    one = 1;
    lifetime = &emitter->particles[0].lifetime;
    slot = emitter->particles;
    do {
        u32 flags;
        Sprite_Init(slot, spriteGraphics, 0);
        flags = slot->sprite.flags | one;
        slot->sprite.flags = flags;
        if (i & one)
            flags |= 0x200;
        else
            flags |= 0x400;
        slot->sprite.flags = flags;
        zero = 0;
        *lifetime = zero;
        slot->sprite.priority = one;
        lifetime = (u16 *)((u32)lifetime + sizeof(*slot));
        slot++;
        i++;
    } while (i <= 11);
    *update = zero;
    *age = zero;
    return zero;
}

struct SabataFrost *SabataFrost_CreateTask(void)
{
    struct SabataFrost *r = Task_Create(8, 0x3a8);
    if (r != 0) {
        Task_SetCallbacks(r, SabataFrost_Update, SabataFrost_Destroy);
        if (SabataFrost_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}

struct SabataFrost *SabataFrost_Create(void)
{
    struct SabataFrost *emitter = SabataFrost_CreateTask();
    emitter->source = 0;
    emitter->emitting = 0;
    emitter->runtime.update = 0;
    emitter->runtime.age = 0;
    return emitter;
}

void SabataFrost_StartEmission(struct SabataFrost *emitter, struct ContactPosition *source)
{
    if (emitter) {
        void (*f)(struct SabataFrost *);
        emitter->source = source;
        emitter->emitting = 1;
        f = SabataFrost_Step;
        emitter->runtime.update = f;
        emitter->runtime.age = 0;
    }
}

void SabataFrost_StopEmission(struct SabataFrost *emitter)
{
    if (emitter)
        emitter->emitting = 0;
}
