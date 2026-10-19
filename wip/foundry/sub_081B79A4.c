#define AnimatedMarker_TickActive    sub_081B79A4
#define AnimatedMarker_BeginCooldown sub_081B7920
#include "global.h"
/* Tentative proximity-marker family; collider extent interpretation is provisional. */
struct AnimatedMarkerCollider {
    u8 pad0[6];
    u16 flags;
    u16 pad8;
    u16 extent;
};
struct AnimatedMarker {
    u32 flags;
    u8 pad4[3];
    u8 mode;
    u8 pad8[8];
    u16 timer;
    u8 pad12[0x48 - 0x12];
    struct AnimatedMarkerCollider collider;
    u8 pad54[0xf4 - 0x54];
    u16 phaseTimer;
    u16 padf6;
    u16 animationTimer;
    u8 padfa[8];
    u8 state;
    u8 pad103;
    u8 cellRef[0x10];
    void (*update)(struct AnimatedMarker *);
};
void Sound_PlaySong(u32);
void OrbitingParticles_Create(void *, int, int, int, int);
void sub_081B7920(struct AnimatedMarker *);
void sub_081B79A4(struct AnimatedMarker *marker)
{
    u16 *timer;
    marker->mode = 2;
    timer = &marker->phaseTimer;
    if (*timer == 0) {
        Sound_PlaySong(426);
        OrbitingParticles_Create((u8 *)marker + 0x1c, 256, 32, 16, 7);
    }
    ++*timer;
    if (*timer <= 7)
        marker->timer = 1;
    else
        sub_081B7920(marker);
}
