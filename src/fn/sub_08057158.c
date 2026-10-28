// MATCHING_CONCESSION: register-pinning
#include "level_advance_display.h"
void LevelAdvanceDisplay_UpdateAnimation(struct LevelAdvanceManager *manager)
{
    u16 *active = &manager->active;
    u16 *kind;
    u16 *kindCache;
    u32 kindValue;
    u32 timer;
    register u32 left asm("r0"), right;
    if (!*active)
        return;
    kind = &manager->kind;
    kindValue = *kind;
    kindCache = kind;
    asm("" : : "r"(kindCache), "r"(kindValue));
    if (kindValue == 1) {
        register u16 *frame asm("r6") = &manager->frame;
        struct LevelAdvanceParticle *particle = manager->particles;
        s32 i;
        u32 zero;
        i = 7;
        do {
            if (particle->active)
                LevelAdvanceDisplay_UpdateParticle(particle, manager->particleResource);
            particle++;
        } while (--i >= 0);
        {
            register u32 sample asm("r2") = *frame;
            u32 shifted;
            shifted = sample << 16;
            if ((shifted >> 16) <= 63 && !(sample & 3)) {
                shifted >>= 18;
                shifted &= 7;
                LevelAdvanceDisplay_StartParticle(&manager->particles[shifted]);
            }
        }
        {
            u32 next = *frame + 1;
            zero = 0;
            *frame = next;
        }
        manager->sprite.position = gPlayers[0]->position;
        timer = *frame;
        if (timer <= 119) {
            {
                struct LevelAdvanceSprite *sprite = &manager->sprite;
                sprite->frame = zero;
            }
            timer = *frame;
            if (timer <= 4) {
                left = (u16)manager->sprite.position.world.y + 250;
                right = *frame * 30;
                asm("" : "+r"(right));
                goto add_height;
            }
            goto rise;
        }
        if (timer <= 122) {
            {
                u32 amount = 400;
                left = amount;
                asm("" : "+r"(left) : "r"(amount));
                right = (u16)manager->sprite.position.world.y;
                manager->sprite.position.world.y = left + right;
            }
            manager->sprite.frame = 1;
        } else if (timer <= 125) {
            {
                u32 amount = 400;
                left = amount;
                asm("" : "+r"(left) : "r"(amount));
                right = (u16)manager->sprite.position.world.y;
                manager->sprite.position.world.y = left + right;
            }
            manager->sprite.frame = 2;
        } else {
            manager->sprite.flags |= 1;
            manager->active = zero;
            *kindCache = zero;
        }
    } else if (kindValue == 2) {
        register u16 *frame asm("r2") = &manager->frame;
        u32 zero;
        {
            u32 next = *frame + 1;
            zero = 0;
            *frame = next;
        }
        manager->sprite.position = gPlayers[0]->position;
        timer = *frame;
        if (timer <= 119) {
            manager->sprite.frame = 3;
            timer = *frame;
            if (timer <= 4) {
                left = (u16)manager->sprite.position.world.y + 250;
                right = *frame * 30;
                goto add_height;
            }
            goto rise;
        /* Both notification kinds share the same rise and shake phase. */
        rise:
            if (timer <= 19) {
                const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
                u32 *randomIndex = &gRandomIndex;
                register u32 index asm("r1") = *randomIndex + 1;
                u32 mask = 1023;
                u32 rangeMask;
                register s32 value asm("r2");
                register u32 coordinate asm("r0");
                index &= mask;
                value = table[index];
                coordinate = (u16)manager->sprite.position.world.x;
                coordinate -= 15;
                rangeMask = 31;
                value &= rangeMask;
                manager->sprite.position.world.x = coordinate + value;
                index = (index + 1) & mask;
                value = table[index];
                coordinate = (u16)manager->sprite.position.world.y;
                coordinate -= 15;
                value &= rangeMask;
                manager->sprite.position.world.y = coordinate + value;
                index = (index + 1) & mask;
                *randomIndex = index;
                {
                    s32 lastValue = table[index];
                    coordinate = (u16)manager->sprite.position.world.z;
                    coordinate -= 15;
                    lastValue &= rangeMask;
                    manager->sprite.position.world.z = coordinate + lastValue;
                }
            }
            {
                u32 amount = 400;
                left = amount;
                asm("" : "+r"(left) : "r"(amount));
            }
            right = (u16)manager->sprite.position.world.y;
        add_height:
            manager->sprite.position.world.y = left + right;
            return;
        }
        if (timer <= 122) {
            {
                u32 amount = 400;
                left = amount;
                asm("" : "+r"(left) : "r"(amount));
                right = (u16)manager->sprite.position.world.y;
                manager->sprite.position.world.y = left + right;
            }
            manager->sprite.frame = 4;
        } else if (timer <= 125) {
            {
                u32 amount = 400;
                left = amount;
                asm("" : "+r"(left) : "r"(amount));
                right = (u16)manager->sprite.position.world.y;
                manager->sprite.position.world.y = left + right;
            }
            manager->sprite.frame = kindValue;
        } else {
            manager->sprite.flags |= 1;
            *active = zero;
            *kind = zero;
        }
    } else {
        manager->sprite.flags |= 1;
        *active = 0;
        *kind = 0;
    }
    return;
}
