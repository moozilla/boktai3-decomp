#include "player/effects/player_attack_particles.h"
#include "engine/engine.h"

extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
s32 Div(s32, s32);
static inline s32 shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
#define SIN(n) gUnk_086149C4[(n) & 255]
static inline s32 shift_distance(s32 x)
{
    if (x >= 0)
        x = x >> 12;
    else
        x = -((-x) >> 12);
    return x;
}
static inline u16 mask8(s32 x)
{
    return x & 255;
}
s32 PlayerAttackEffect_EmitPaletteBaseTrail(struct AttackParticleEffect *p)
{
    struct ContactPosition a, b, c;
    u32 state = p->current;
    if (state == 1) {
        s32 step, angle;
        if (p->frame == 0) {
            gRandomIndex = (gRandomIndex + 1) & 1023;
            p->radius = Mod(*(u16 *)((gRandomIndex << 1) + RANDOM_TABLE_ADDR), 10) + 230;
        }
        angle = ((*p->facing + 5) & 7) << 5;
        step = Div(768, p->owner->durationScale);
        if (p->owner->frameIndex > 1)
            goto done;
        if (p->owner->elapsed > 2)
            goto done;
        {
            s32 a6, m;
            if (*p->side) {
                s32 t = p->frame * step;
                s32 h = t + step / 2;
                s32 k = -256;
                a6 = angle - (h + k) + 32;
                m = 255;
                a6 &= m;
                angle = angle - (t + k) + 32;
            } else {
                s32 t = p->frame * step;
                s32 h = t + step / 2;
                s32 k = 256;
                a6 = angle + (h + k) - 32;
                m = 255;
                a6 &= m;
                angle = angle + (t + k) - 32;
            }
            angle = angle & m;
            a.x = p->owner->position.x + shift12(p->radius * SIN(angle + 64));
            a.y = p->owner->position.y + 200;
            a.z = p->owner->position.z + shift12(p->radius * gUnk_086149C4[angle]);
            PlayerAttackEffect_AllocateParticle(p, &a, 5, &p->graphics[0], 0, 0);
            a.x = p->owner->position.x + shift12(p->radius * SIN(a6 + 64));
            a.y = p->owner->position.y + 200;
            a.z = p->owner->position.z + shift12(p->radius * gUnk_086149C4[a6]);
            PlayerAttackEffect_AllocateParticle(p, &a, 5, &p->graphics[0], 0, 0);
        }
        p->frame++;
    } else if (state == 2) {
        if (p->frame == 0)
            p->radius = 290;
        if ((u16)(p->owner->frameIndex - 2) > 2)
            goto done;
        {
            s32 angle;
            s32 a64;
            s32 pitch, distance;
            angle = ((*p->facing + 5) & 7) * 32;
            pitch = (306 - p->frame * 10) & 255;
            b.y = p->owner->position.y + shift12(p->radius * SIN(pitch)) + 190;
            distance = shift_distance(p->radius * SIN(pitch + 64));
            {
                s32 temporary = angle + 64;
                s32 product;
                product = distance * SIN(temporary);
                a64 = temporary;
                b.x = p->owner->position.x + shift12(product) - 16;
            }
            b.z = p->owner->position.z + shift12(distance * SIN(angle)) + 16;
            PlayerAttackEffect_AllocateParticle(p, &b, 5, &p->graphics[0], 0, 0);
            pitch = (311 - p->frame * 10) & 255;
            b.y = p->owner->position.y + shift12(p->radius * SIN(pitch)) + 190;
            distance = shift_distance(p->radius * SIN(pitch + 64));
            b.x = p->owner->position.x + shift12(SIN(a64) * distance) - 16;
            b.z = p->owner->position.z + shift12(distance * SIN(angle)) + 16;
            PlayerAttackEffect_AllocateParticle(p, &b, 5, &p->graphics[0], 0, 0);
        }
        p->frame++;
    } else if (state == 3) {
        s32 angle;
        if (p->frame == 0)
            p->radius = 60;
        if (p->owner->frameIndex > 2)
            p->radius -= 70;
        else
            p->radius += 35;
        angle = ((*p->facing + 5) & 7) * 32;
        c.x = p->owner->position.x + shift12(p->radius * SIN(angle + 64)) - 16;
        c.y = p->owner->position.y + 200;
        c.z = p->owner->position.z + shift12(p->radius * gUnk_086149C4[angle]) + 16;
        PlayerAttackEffect_AllocateParticle(p, &c, 5, &p->graphics[0], 0, 0);
        p->frame++;
    } else
        p->frame = 0;
done:
    return 0;
}
