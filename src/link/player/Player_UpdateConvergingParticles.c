#include "effects/orbits/converging_effects.h"
#include "monsters/ghoul_disc.h"

/* Tentative role: update four effects moving toward a common center. */
extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);
void SpriteDrawable_SelectTileFrame(struct ConvergingParticle *, void *, s32);

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

void Player_UpdateConvergingParticles(struct Player *player)
{
    s32 count, i;
    if (!player->convergingEffects.active)
        return;
    count = 0;
    for (i = 0; i < 4; i++) {
        struct ConvergingParticle *e = &player->convergingEffects.particles[i];
        if (e->active) {
            struct ConvergingParticle *draw = e;
            if (Ghoul_Abs(draw->offsetX) <= 15 && Ghoul_Abs(draw->offsetZ) <= 15) {
                draw->flags |= 1;
                e->active = 0;
            } else {
                s32 angle = Math_Atan2(e->offsetX, e->offsetZ);
                e->offsetX -= scale(e->speed * gUnk_086149C4[(angle + 64) & 255]);
                e->offsetZ -= scale(e->speed * gUnk_086149C4[angle & 255]);
                draw->position.x = player->actionOrigin.x + (u16)e->offsetX;
                draw->position.z = player->actionOrigin.z + (u16)e->offsetZ;
                if (++e->ticks == 4)
                    SpriteDrawable_SelectTileFrame(draw, player->convergingEffects.graphics,
                                                   e->kind + 1);
                count++;
            }
        }
    }
    if (!count)
        player->convergingEffects.active = 0;
}
