#include "global.h"
#include "scene_vec.h"
#include "monster_particle_random.h"
#include "game_state.h"
#include "engine_globals.h"
#include "ram.h"
/* The sun gauge controls how quickly a new particle is emitted into the four-slot ring. */
typedef struct SceneVec Point;
typedef struct Effect {
    u32 flags;
    u8 pad4[0x14];
    Point pos;
    u8 pad20[8];
    u8 active, ticks, speed, kind;
    s16 dx;
    u8 pad2e[2];
    s16 dz;
    u8 pad32[2];
} Effect;
typedef struct State {
    u8 pad0[0x9a8];
    void *graphics;
    u8 active, next, ticks, pad9af;
    Effect effects[4];
} State;
extern const s16 gUnk_086149C4[];
void SpriteDrawable_SelectTileFrame(Effect *, void *, s32);
static inline s32 ReadParticleSpeedSample(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)((u8 *)RANDOM_TABLE_ADDR + (gRandomIndex << 1));
}
static inline s32 ScaleParticleOffset(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void ConvergingEffects_EmitNearParticle(State *s, Point *pos)
{
    s32 step = gGameState->unk876;
    s->ticks = step + 2 + s->ticks;
    if (s->ticks > 39) {
        Effect *e = &s->effects[s->next];
        Effect *draw = e;
        s32 angle;
        s32 radius;
        u16 radiusSample;
        draw->flags &= ~1;
        e->kind = 2;
        SpriteDrawable_SelectTileFrame(draw, s->graphics, 2);
        angle = ParticleRandom_ReadDirectSample(gRandomTable) & 255;
        radiusSample = ParticleRandom_ReadDirectSample(gRandomTable);
        radius = radiusSample % 64 + 128;
        e->dx = ScaleParticleOffset(radius * gUnk_086149C4[(angle + 64) & 255]);
        e->dz = ScaleParticleOffset(radius * gUnk_086149C4[angle & 255]);
        draw->pos = *pos;
        draw->pos.x += e->dx;
        draw->pos.z += e->dz;
        e->speed = (ReadParticleSpeedSample() & 3) + 14;
        e->ticks = 0;
        e->active = 1;
        if (++s->next > 3)
            s->next = 0;
        s->ticks -= 40;
        s->active = 1;
    }
}
