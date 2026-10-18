#define SpriteBurst_Init sub_08237E40
#define SpriteBurst_InitSprite ActorGlyphTrail_InitSprite
#define SpriteBurst_InitParticles sub_08237DD4
#define SpriteBurst_SetCallback ActorGlyphTrail_SetState
#include "global.h"
struct SpriteBurst {
    u8 unknown0[0x18];
    u32 parameter;
    u8 unknown1C[0x50];
    u16 value;
    u8 unknown6E, state;
    u8 unknown70[2];
    u16 variant;
};
void ActorGlyphTrail_InitSprite(struct SpriteBurst *);
void sub_082377B8(struct SpriteBurst *);
void sub_08237DD4(struct SpriteBurst *);
void sub_08237BD4(void);
void ActorGlyphTrail_SetState(struct SpriteBurst *, void (*)(void));
s32 sub_08237E40(struct SpriteBurst *p, u32 parameter, u32 value, u32 variant)
{
    p->parameter = parameter;
    p->value = value;
    p->variant = variant;
    p->state = 0;
    ActorGlyphTrail_InitSprite(p);
    sub_082377B8(p);
    sub_08237DD4(p);
    ActorGlyphTrail_SetState(p, sub_08237BD4);
    return 0;
}
