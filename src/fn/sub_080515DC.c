// MATCHING_CONCESSION: register-pinning
#include "breakable_prop.h"
void BreakableProp_EmitBreakParticles(struct BreakableProp *prop)
{
    struct {
        struct AnimatedParticleConfig config;
        union {
            struct ContactPosition axes;
            u32 words[2];
        } position;
    } local;
    struct ContactPosition *positionCache;
    struct ContactPosition *positionProxy;
    s32 i;
    u32 mask;
    s32 range;
    u32 *indexAddress;
    register const u16 *random asm("r4");
    local.config.type = 2;
    local.config.flags = 0;
    local.config.speed = 64;
    local.config.vertical = 3;
    local.config.verticalRange = 6;
    local.config.horizontal = 2;
    local.config.horizontalRange = 2;
    local.config.pitch = 60;
    local.config.pitchRange = 8;
    local.config.yaw = 0;
    local.config.yawRange = 255;
    local.config.verticalStart = 255;
    local.config.verticalStartRange = 0;
    local.config.verticalDecay = 255;
    local.config.verticalDecayRange = 0;
    local.config.horizontalStart = 255;
    local.config.horizontalStartRange = 0;
    local.config.horizontalDecay = 255;
    local.config.horizontalDecayRange = 0;
    positionCache = &local.position.axes;
    indexAddress = &gRandomIndex;
    mask = 1023;
    range = 300;
    i = 5;
    positionProxy = positionCache;
    do {
        local.position.axes = prop->sprite.position;
        random = (const u16 *)RANDOM_TABLE_ADDR;
        *indexAddress = (*indexAddress + 1) & mask;
        {
            s32 sample = Mod(random[*indexAddress], range);
            register u32 x asm("r1") = (u16)positionCache->x;
            x -= 150;
            x += sample;
            x = (u16)x;
            local.position.words[0] = (local.position.words[0] & 0xFFFF0000) | x;
        }
        *indexAddress = (*indexAddress + 1) & mask;
        {
            s32 sample = Mod(random[*indexAddress], range);
            u32 packet = local.position.words[0];
            s32 y = (s32)packet >> 16;
            u32 lower;
            y += sample;
            y = (u32)y << 16;
            lower = 0xFFFF;
            lower &= packet;
            lower |= y;
            local.position.words[0] = lower;
        }
        *indexAddress = (*indexAddress + 1) & mask;
        {
            s32 sample = Mod(random[*indexAddress], range);
            register u32 z asm("r1") = (u16)positionProxy->z;
            z -= 150;
            z += sample;
            z = (u16)z;
            ((u32 *)positionProxy)[1] = (((u32 *)positionProxy)[1] & 0xFFFF0000) | z;
        }
        AnimatedParticles_Create(1, positionProxy, &local.config);
    } while (--i >= 0);
}
