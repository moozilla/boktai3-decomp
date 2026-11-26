#include "global.h"
struct AnimatedMarker {
    u8 pad0[7];
    u8 mode;
    u8 pad8[8];
    u16 visible;
    u8 pad12[0x1C - 0x12];
    u8 position[0xF4 - 0x1C];
    u16 timer;
};
void Sound_PlaySong(u32);
void OrbitingParticles_Create(void *, s32, s32, s32, s32);
void GrowthHazard_BeginCooldown(struct AnimatedMarker *);
// confidence=low: on the first tick plays sound 0x1AA and spawns orbiting particles; after eight
// ticks starts the cooldown.
void AnimatedMarker_TickActive(struct AnimatedMarker *p)
{
    p->mode = 2;
    if (p->timer == 0) {
        Sound_PlaySong(0x1AA);
        OrbitingParticles_Create(p->position, 0x100, 0x20, 0x10, 7);
    }
    if (++p->timer <= 7)
        p->visible = 1;
    else
        GrowthHazard_BeginCooldown(p);
}
