#include "global.h"

#ifdef NONMATCHING
// fuzzy: 83.3%
// foundry draft (worker best 83.3%); what differs is mostly register allocation and instruction order
#define AnimatedMarker_BeginStateThree sub_081B7954
struct AnimatedMarkerSprite {
    u8 pad0[6];
    u16 flags;
    u16 pad8;
    u16 scale;
};
struct AnimatedMarker {
    u32 flags;
    u8 pad4[3];
    u8 mode;
    u8 pad8[8];
    u16 timer;
    u8 pad12[0x48 - 0x12];
    struct AnimatedMarkerSprite sprite;
    u8 pad54[0xf4 - 0x54];
    u16 phaseTimer;
    u16 padf6;
    u16 animationTimer;
    u8 padfa[8];
    u8 state;
    u8 pad103;
    u8 animation[0x10];
    void (*update)(struct AnimatedMarker *);
};
void sub_081B7A2C(struct AnimatedMarker *);
static inline void AnimatedMarker_SetStateThree(struct AnimatedMarker *marker,
                                                void (*update)(struct AnimatedMarker *), u32 offset)
{
    *(void (**)(struct AnimatedMarker *))((u8 *)marker + offset) = update;
    marker->phaseTimer = 0;
    *((u8 *)marker + (offset - 0x12)) = 3;
}
void sub_081B7954(struct AnimatedMarker *marker)
{
    AnimatedMarker_SetStateThree(marker, sub_081B7A2C, 0x114);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081B7954);
#endif
