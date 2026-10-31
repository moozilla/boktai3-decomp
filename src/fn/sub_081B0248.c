#include "global.h"
#include "scene_vec.h"
#include "engine.h"
#include "monster_particle_random.h"

struct VanargandPart {
    u8 unk_00[0x90];
    struct SceneVec *basePosition;
    u8 unk_94[0x15c - 0x94];
    u32 variant;
};
extern const s16 gUnk_086149C4[];
void *VanargandEffect_Create(s32, s32, struct SceneVec *, struct SceneVec *, u32);

static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void Vector(struct SceneVec *out, s32 angle, s32 radius)
{
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void Add(struct SceneVec *out, struct SceneVec *base, struct SceneVec *offset)
{
    out->x = offset->x + base->x;
    out->y = offset->y + base->y;
    out->z = offset->z + base->z;
}

void Vanargand_SpawnPartEmergenceEffects(void *arg0)
{
    struct VanargandPart *part = arg0;
    struct SceneVec position;
    struct SceneVec velocity;
    s32 i;
    for (i = 0; i < 4; i++) {
        s32 angle;
        s32 offset, radius;
        const u16 *table;
        u32 *index;
        s32 sample, sampleRadius, shifted;
        if (part->variant == 0)
            angle = (i << 3) + 48;
        else
            angle = 255 - (i << 3);
        Vector(&position, angle, 640);
        Add(&position, part->basePosition, &position);
        position.y += 256;
        table = gRandomTable;
        index = &gRandomIndex;
        *index = (*index + 1) & 1023;
        PARTICLE_RANDOM_READ_SAMPLE(sample, table, *index);
        shifted = sample >> 3;
        offset = shifted - shifted / 32 * 32;
        *index = (*index + 1) & 1023;
        PARTICLE_RANDOM_READ_SAMPLE(sampleRadius, table, *index);
        radius = (sampleRadius >> 3) % 8;
        Vector(&velocity, angle + offset, radius + 12);
        VanargandEffect_Create(0, 0, &position, &velocity, 1 - part->variant);
    }
}
