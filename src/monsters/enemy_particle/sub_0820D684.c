#include "engine/math/math_shift.h"
#include "monsters/enemy_particle.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/ram.h"
extern const s16 gUnk_086149C4[];
void EnemyParticle_CreatePool(void);
struct EnemyParticle *EnemyParticle_FindFreeSlot(struct EnemyParticlePool *, s32 *);
void Memory_Copy(void *, const void *, s32);
s32 Mod(s32, s32);
void Sprite_LinkToList(void *, u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);

s32 EnemyParticle_Emit(struct EnemyParticleParams *params, struct ContactPosition *pos)
{
    struct EnemyParticlePool *pool = gEnemyParticlePool;
    struct EnemyParticle *e;
    s32 slot;
    struct ContactSwarmSprite *o;
    void *ad;
    struct ContactPosition *v;
    s32 magnitude;
    s32 angle;
    if (!pool) {
        EnemyParticle_CreatePool();
        return -1;
    }
    e = EnemyParticle_FindFreeSlot(pool, &slot);
    if (!e)
        return -1;
    Memory_Copy(&e->params, params, sizeof(*params));
    e->timer = 0;
    v = &e->velocity;
    angle = params->angle;
    magnitude = params->speed;
    {
        s32 scaled = Math_ShiftTowardZero(magnitude * gUnk_086149C4[(angle + 0x40) & 0xff], 12);
        v->x = scaled;
    }
    v->y = 0;
    {
        s32 scaled = Math_ShiftTowardZero(magnitude * gUnk_086149C4[angle & 0xff], 12);
        v->z = scaled;
    }
    e->sprite.position = *pos;
    ad = e->graphics;
    AnimData_Load(ad, params->resource);
    {
        struct ContactSwarmSprite *s = &e->sprite;
        s32 bank;
        s->frame = params->frame;
        bank = params->bank;
        o = s;
        if (bank >= 0)
            AnimData_SelectPaletteBank(ad, bank);
    }
    e->sprite.flags &= ~1;
    e->sprite.flags |= params->spriteFlags;
    Sprite_LinkToList(o, 0);
    if (e->params.script) {
        e->scriptProc = ScriptProc_Start(0x922E, (u16)e->params.script);
        if (e->params.mode == 0)
            Anim_SetSequence(&e->animation, (void *)e->scriptProc, (u16)e->params.sequence, 0, 0);
        else if (e->params.mode == 4) {
            u32 dir = (((u16)e->params.angle + 0x60) >> 6) & 3;
            Anim_SetSequence(&e->animation, (void *)e->scriptProc, (u16)e->params.sequence, dir, 0);
        } else {
            u32 dir = (((u16)e->params.angle + 0x60) >> 6) & 3;
            Anim_SetSequence(&e->animation, (void *)e->scriptProc, (u16)e->params.sequence, dir, 0);
        }
        PlayerCue_StepSequence(&e->sprite, &e->animation);
    }
    e->params.groundHeight = e->sprite.position.y;
    if (params->shadow)
        e->sprite.flags |= 0x200;
    pool->mask |= 1 << slot;
    return 0;
}
