#include "global.h"
#ifdef NONMATCHING
// fuzzy: 92.0%
#include "background_controller.h"
#include "contact_position.h"
#include "monster_particle_random.h"

struct Camera {
    u8 pad[0x44];
    s32 x;
};
extern const s16 gUnk_086149C4[];
void Actor_UpdateVisibilityAndShadow(u32, u8 *);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
void *SpriteEffectController_Create(const struct ContactPosition *, const struct ContactPosition *,
                                    s32, u32, u32, u32, u32, u32, u32, u32, u32, u32, s32, s32, s32,
                                    s32, s32, s32);
struct SceneNpcEffectState {
    u8 unknown0[0xC];
    u8 active, attached;
    s16 paletteFrame;
};
struct SceneNpcEffect {
    u8 unknown0[6];
    u8 state;
    u8 unknown7[5];
    u16 flags, displayFlags;
    u8 unknown10[0x10];
    struct SceneNpcEffectState effect;
    u8 unknown30[0x68 - 0x30];
    struct ContactPosition position;
    u8 unknown70[0x90 - 0x70];
    u16 size[2];
    u8 unknown94[0x160 - 0x94];
    struct ContactPosition *displayPosition;
    u8 unknown164[0x1cc - 0x164];
    u8 palette[0x20];
    u8 unknown1ec[0x300 - 0x1ec];
    s32 projected[3];
    u8 unknown30c[0x328 - 0x30c];
    s32 height;
};
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

/* Begin the sprite velocity vector with its Q12 horizontal component. */
#define START_PARTICLE_VELOCITY(argument, destination, productX)                                   \
    do {                                                                                           \
        (argument) = (destination);                                                                \
        (destination)->x = Truncate(productX);                                                     \
    } while (0)

s32 SceneNpc_OnStateEffectUpdate(u32 context, u8 *actor)
{
    u32 work = context;
    u8 palettePeriod;
    u8 *p = actor;
    u8 *effectData = p + 0x20;
    struct SceneNpcEffect *npc = (void *)p;
    struct SceneNpcEffectState *fx = (void *)effectData;
    if (effectData[0xd] != 0) {
        s32 zero = 0;
        s32 *pos;
        struct ContactPosition *dst;
        s32 height;
        fx->active = 1;
        pos = (s32 *)(p + 0x300);
        dst = npc->displayPosition;
        height = npc->height;
        dst->x = (pos[0] >> 12) - gSceneCamera->x;
        dst->y = ((pos[2] + (pos[1] + height)) >> 12) + 0x60;
        dst->z = pos[2] >> 12;
        {
            u16 *w = npc->size;
            u16 height;
            *w = 4;
            w++;
            height = 0xFFE8;

            *w = height;
        }
        npc->position = *npc->displayPosition;
        npc->position.z = zero;
        {
            u32 m = 0x200;

            npc->displayFlags = m | npc->displayFlags;
        }
    }
    Actor_UpdateVisibilityAndShadow(work, p);
    if ((fx->active == 0) && (npc->state <= 0x10)) {
        u32 m = 0x402;

        npc->displayFlags = m | npc->displayFlags;
    }
    {
        u32 m = 4;

        if ((m & (*((u16 *)(p + 0xc)))) != 0) {
            s32 t;
            if ((*((s16 *)(effectData + 0xe))) <= 0x3f) {
                u8 *g = gPaletteBankData;
                void *s1 = g + 0x2AE0;
                void *s2 = g + 0x2B00;
                void *d = p + 0x1CC;
                Palette_InterpolateBanks(d, s1, s2, *((s16 *)(effectData + 0xe)), 6);
            } else {
                u8 *g = gPaletteBankData;
                void *s1 = g + 0x2AE0;
                void *s2 = g + 0x2B00;
                void *d = p + 0x1CC;
                Palette_InterpolateBanks(
                    d, s1, s2, (palettePeriod = 0x80) - (*((s16 *)(effectData + 0xe))), 6);
            }
            t = ((*((u16 *)(effectData + 0xe))) + 1) & 0x7f;
            *((u16 *)(effectData + 0xe)) = t;
            if (effectData[0xd] != 0) {
                work = 3;
                if ((t & work) == 0) {
                    u32 sheet = 0x1c1e, sequence = 0xd43f;
                    struct ContactPosition position;
                    struct ContactPosition velocityStorage;
                    s32 *projected = (s32 *)(p + 0x300);
                    struct ContactPosition *pos = &position;
                    s32 height = *((s32 *)(p + 0x328));
                    pos->x = (projected[0] >> 12) - gSceneCamera->x;
                    pos->y = ((projected[2] + (projected[1] + height)) >> 12) + 0x60;
                    pos->z = projected[2] >> 12;
                    {
                        u32 origin;
                        struct ContactPosition *velocityArgument;
                        const u16 *table;
                        struct ContactPosition *velocity;
                        s32 spread, magnitude, angle, speed, product, life, zero;
                        const s16 *sine;
                        PARTICLE_RANDOM_CAPTURE_ORIGIN(origin);
                        table = gRandomTable;
                        spread = ((s32)ParticleRandom_ReadDirectSample(table) >> 4) & 63;
                        magnitude = ((s32)ParticleRandom_ReadDirectSample(table) >> 4) & work;
                        velocity = &velocityStorage;
                        angle = spread + 0x60;
                        speed = magnitude + 2;
                        sine = gUnk_086149C4;
                        product = sine[(spread + 0xA0) & 255] * speed;
                        START_PARTICLE_VELOCITY(velocityArgument, velocity, product);
                        velocity->y = Truncate(sine[angle] * speed);
                        zero = 0;
                        velocity->z = zero;
                        life = (((s32)ParticleRandom_ReadRelativeSample(
                                     origin, (const u16 *)RANDOM_TABLE_ADDR) >>
                                 4) &
                                15) +
                               15;
                        SpriteEffectController_Create(&position, velocityArgument, sheet, sequence,
                                                      zero, 16, 17, 1, 2, 1, zero, life, zero, zero,
                                                      zero, zero, -4, -4);
                    }
                }
            }
        }
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", SceneNpc_OnStateEffectUpdate);
#endif
