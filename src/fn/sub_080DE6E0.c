#include "scene_actor_state.h"
#include "scene_actor_registry.h"
#include "display_sequence.h"
#include "ram.h"

extern const s16 gUnk_086149C4[];
extern const u16 gUnk_086061E4[][8];
void SpriteEffectController_Create(void *, const void *, s32, u32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, s32, s32, s32, s32, s32, s32);

static inline u32 NextRandomSample(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

static inline u16 TestAnimationFlags(struct SceneActorRegistryResetView *actor, u16 mask)
{
    return actor->flags2D4 & mask;
}

static inline void AddPosition(struct ContactPosition *out, const struct ContactPosition *a,
                               const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

static inline s32 TruncateFixed(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void SetDirectionOffset(struct ContactPosition *position, u32 angle, s32 distance)
{
    position->x = TruncateFixed(gUnk_086149C4[(angle + 0x40) & 0xFF] * distance);
    position->y = 0;
    position->z = TruncateFixed(gUnk_086149C4[angle] * distance);
}

static inline void SetStatusFlags(void *object, u32 flags)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->flags2D0 = flags | actor->flags2D0;
}
static inline u32 ReadPaletteOffset(u16 *offset)
{
    return *offset;
}

u16 sub_080DE6E0(struct SceneActorRegistryResetView *actor)
{
    struct DisplaySequenceState *sequence =
        &((struct SceneActorControlMotionDrawable *)actor->drawable)->sequence;
    u32 alternate;
    u16 *paletteOffset;
    u32 palette;
    union SceneActorRegistryPackedPosition position;
    struct ContactPosition offset;
    struct ContactPosition velocity;

    if (sequence->elapsed == 0) {
        alternate = TestAnimationFlags(actor, 0x80) != 0;
        SetStatusFlags(actor, 0x10000);
        palette = ReadPaletteOffset(&actor->paletteOffset);
        paletteOffset = &actor->paletteOffset;
        if (palette == 3 || palette == 1) {
            if (sequence->index == 7 || sequence->index == 5 || sequence->index == 1) {
                position.packed.x = (NextRandomSample(gRandomTable) & 63) - 48;
                position.packed.z = (NextRandomSample(gRandomTable) & 63) - 48;
                AddPosition(&position.position, &position.position, &actor->body.position);
                SetDirectionOffset(&offset, (actor->body.direction + 0x80) & 0xFF, 0x50);
                AddPosition(&position.position, &position.position, &offset);
                position.position.y = actor->body.position.y;
                ZeroFill8(&velocity);
                if (*paletteOffset == 3) {
                    s32 lifetime = (NextRandomSample((const u16 *)RANDOM_TABLE_ADDR) & 63) + 20;
                    SpriteEffectController_Create(&position, &velocity, 0x1C1A, 0xD1B8, 3, 0, 0x31,
                                                  0, 1, 0, 0, lifetime, 4, 1, 0x200004, 30, 8, 8);
                } else {
                    SpriteEffectController_Create(&position, &velocity, 0x1C1A, 0xD1B8, 0x12, 0,
                                                  0x11, 0, 1, 0, 0, 0, 4, 1, 0x400008, 30, 8, 8);
                }
            }
        }
        return gUnk_086061E4[alternate][sequence->index & 7];
    }
    return 0;
}
