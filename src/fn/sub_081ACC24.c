#include "global.h"
#include "vanargand_pair_attack.h"
#include "vanargand_stage_colors.h"
void Sound_PlaySong(u32);
void CpuSet(const void *, void *, u32);



static inline void setFlags(struct VanargandPairAttack *p, u32 mask)
{
    p->flags |= mask;
}
static inline void setPosition(s16 *dst, s32 x, s32 y, s32 z)
{
    s16 *first = dst;
    *first = x;
    dst[1] = y;
    dst[2] = z;
}
static inline u32 partFlags(struct VanargandPairAttack *p)
{
    return p->part.contactFlags;
}
// confidence=low: distinguishes accumulated body damage from paired-part contact damage.
void DamageNumberDisplay_ShowWithDefaultScale(void *, s32, s32, void *, s32);
void Vanargand_UpdatePaletteLeap(void *, s32);
static inline s16 ReadAccumulatedDamage(struct VanargandPairAttack *actor, s16 **field)
{
    *field = &actor->accumulatedDamage;
    return **field;
}

s32 Vanargand_ApplyPendingDamage(struct VanargandPairAttack *p)
{
    u32 mask;
    s16 *hitTotal;
    struct VanargandPairAttack *self = p;
    void *display = &p->damageSource;
    s32 damage = p->damage;
    u32 special = (p->hitFlags >> 6) & 1;
    s32 partDamage;
    u32 partSpecial;
    p->damage = 0;
    p->hitFlags = 0;
    if ((u8)p->part.flags & 6) {
        partDamage = p->part.damage;
        partSpecial = (partFlags(p) >> 6) & 1;
    } else {
        partDamage = 0;
        partSpecial = 0;
    }
    if (damage > 0 || special) {
        p->health -= damage;
        self->accumulatedDamage += damage;
        DamageNumberDisplay_ShowWithDefaultScale(display, damage, 0, &self->animation.position, 1);
        self->flashTimer = 15;
        CpuSet(gPaletteBankData + 0x3240, self->palettes, 0x04000008);
        Vanargand_ResetPaletteTransition(self);
        {
            u32 song = 0x13E;
            if (special)
                song += 0xD3;
            Sound_PlaySong(song);
        }
        if (p->health <= 0) {
            p->health = 0;
            setFlags(p, 2);
            return 1;
        }
        {
            s32 accumulated = ReadAccumulatedDamage(self, &hitTotal);
            s32 transition = self->transition;
            if (transition > 2)
                transition = 2;
            if (accumulated >= self->thresholds[transition]) {
                s32 count = ScenePair_CountPositiveCounters(self->pair);
                if (count == 0) {
                    mask = 1;
                    setFlags(p, mask);
                    *hitTotal = count;
                    return 1;
                } else {
                    s32 transition = self->transition;
                    if (transition > 2)
                        transition = 2;
                    *hitTotal = self->thresholds[transition] - 1;
                }
            } else {
                self->delay += 6;
            }
        }
    }
    if (partDamage > 0 || partSpecial) {
        s16 pos[4];
        p->health -= partDamage;
        setPosition(pos, self->part.position.x + 64, self->part.position.y,
                    self->part.position.z + 64);
        DamageNumberDisplay_ShowWithDefaultScale(&self->part.damageSource, partDamage, 0, pos, 1);
        if (p->health <= 0) {
            setFlags(p, 2);
        } else if (self->part.flags & 4) {
            Vanargand_StartReaction(p, 20, Vanargand_UpdatePaletteLeap);
            mask = 0x10000;
            setFlags(p, mask);
            return 1;
        }
    }
    return 0;
}
