#include "global.h"

// foundry draft (worker best 83.3%); what differs is mostly register allocation and instruction order
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
void GrowthHazard_RevealWhenCellVacant(struct AnimatedMarker *);
static inline void AnimatedMarker_SetStateThree(struct AnimatedMarker *marker,
                                                void (*update)(struct AnimatedMarker *), u32 offset)
{
    *(void (**)(struct AnimatedMarker *))((u8 *)marker + offset) = update;
    marker->phaseTimer = 0;
    *((u8 *)marker + (offset - 0x12)) = 3;
}
static inline void AnimatedMarker_SetStateThree_1(struct AnimatedMarker *marker,
                                                  void (*update)(struct AnimatedMarker *))
{
    *((void (**)(struct AnimatedMarker *))(((u8 *)marker) + 0x114)) = update;
    marker->phaseTimer = 0;
    {
        u8 k1 = 3;
        *(((u8 *)marker) + (0x114 - 0x12)) = k1;
    }
}

void AnimatedMarker_BeginStateThree(struct AnimatedMarker *marker)
{
    AnimatedMarker_SetStateThree_1(marker, GrowthHazard_RevealWhenCellVacant);
}
