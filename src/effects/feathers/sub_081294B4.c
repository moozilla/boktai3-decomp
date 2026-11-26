#include "global.h"
#include "player/cues/player_cue_sequence.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "engine/engine_pause.h"
#include "ui/hud/scene_indicator.h"
struct Feather {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct Vec position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    u8 unknown3C[0x54];
    u16 age;
    u8 state;
    s8 slot;
    u16 blinkAge;
    u8 enter, angle;
    u32 effectFlags, variant;
};
struct FeatherPool {
    u8 task[0x18];
    u8 graphics[0x1c];
    void *sequences;
    u32 used;
};
void Sprite_Remove(void *);
static inline void SelectDirection(u32 angle, u8 *row, u8 *flip)
{
    u8 quadrant = ((((s32)(angle >> 24)) + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *flip = 1;
        *row = 3 - quadrant;
    } else {
        *flip = 0;
        *row = quadrant;
    }
}

extern u32 gUnk_02000560;
struct FeatherPool *FeatherEffectPool_GetOrCreate(void);
struct Feather *FeatherEffectPool_AllocEntry(struct FeatherPool *);
static inline u32 FeatherBlockedSlots(u32 mask)
{
    u32 bits = gUnk_02000560;
    bits &= mask;
    return bits;
}
static inline u32 FeatherCreationBlockFlags(void)
{
    u32 blocked = Engine_PauseBits(0x400);
    if (blocked)
        return 0x400;
    blocked = FeatherBlockedSlots(4);
    return blocked;
}
// confidence=low: creates a directional feather effect with palette and scatter variants.
struct Feather *FeatherEffectPool_CreateDirectional(struct Vec *position, u32 angle, u32 flags,
                                                    u32 variant)
{
    struct FeatherPool *pool = gFadingSpritePool;
    struct Feather *p;
    struct Feather *sprite;
    u8 row, flip;
    if (!pool) {
        pool = FeatherEffectPool_GetOrCreate();
        if (!pool)
            return 0;
    }
    if (FeatherCreationBlockFlags())
        return 0;
    p = FeatherEffectPool_AllocEntry(pool);
    if (!p)
        return 0;
    p->age = 0;
    p->angle = angle;
    p->enter = 1;
    p->state = 2;
    sprite = p;
    sprite->position = *position;
    SelectDirection(angle << 24, &row, &flip);
    Sprite_Init(sprite, pool->graphics, 0);
    Anim_SetSequence(&p->sequence, pool->sequences, 1, row, flip);
    p->variant = variant;
    if (!variant) {
        AnimData_SelectPaletteBank(sprite->graphics, 0x278);
        p->effectFlags = 0x200004;
    } else {
        AnimData_SelectPaletteBank(sprite->graphics, 0x277);
        p->effectFlags = 0x400008;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)sprite, &p->sequence);
    {
        u32 combined = sprite->flags | 1;
        sprite->priority = 1;
        combined |= flags;
        sprite->flags = combined;
    }
    return p;
}
