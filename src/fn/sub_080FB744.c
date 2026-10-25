#include "scene_actor_palette.h"

struct AxePaletteTransition {
    u16 colors[16];
    u16 remaining;
    u16 unk_22;
    s16 red[16], green[16], blue[16];
    s16 deltaRed[16], deltaGreen[16], deltaBlue[16];
};
struct AxePaletteContext {
    u8 unk_0[0x624];
    struct AxePaletteTransition transition;
    u8 unk_708[0x744];
    u32 flags;
};
void Axe_SetAuxiliarySpriteResource(struct SceneActorRegistryActor *, const u16 *);

static inline u32 ReadMasked(u32 mask, const u32 *field)
{
    return *field & mask;
}
static inline u32 ActorFlag(struct SceneActorRegistryActor *actor, u32 mask, u32 offset)
{
    if (offset == 0x2D4)
        return actor->flags2D4 & mask;
    return actor->flags2D0 & mask;
}
static inline u8 Unavailable(struct SceneActorRegistryActor *actor)
{
    if (actor &&
        (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline s32 ChannelValue(s32 value)
{
    if (value >= 0)
        return value >> 8;
    return -(-value >> 8);
}
static inline void BindPalette(struct SceneActorRegistryActor *actor,
                               struct AxePaletteTransition *p)
{
    ((struct SceneActorPaletteSequenceView *)actor->drawable)->palette = p->colors;
    Axe_SetAuxiliarySpriteResource(actor, p->colors);
}
#ifdef NONMATCHING
// fuzzy: 59.8%
// scope shape 64 (differences once low registers are set aside)
// confidence=low: fades the 16-color palette one step toward its targets and binds it to the drawable when the fade ends.
u32 sub_080FB744(struct SceneActorRegistryActor *actor)
{
    struct AxePaletteContext *context;
    struct AxePaletteTransition *p;
    u16 *color;
    s16 *red, *green, *blue;
    s16 *deltaRed, *deltaGreen, *deltaBlue;
    s32 i;
    if (Unavailable(actor))
        return 0;
    context = actor->context;
    if (!Test(ReadMasked(2, &context->flags), ~0))
        return 0;
    p = &context->transition;
    if (!p->remaining) {
        if (!ReadMasked(0x02008000, &actor->flags2CC))
            BindPalette(actor, p);
        return 1;
    }
    red = p->red;
    deltaRed = p->deltaRed;
    deltaGreen = p->deltaGreen;
    deltaBlue = p->deltaBlue;
    color = p->colors;
    blue = p->blue;
    green = p->green;
    i = 0;
    do {
        s32 r, g, b;
        *red += *(s16 *)((u8 *)deltaRed + i);
        *green += *(s16 *)((u8 *)deltaGreen + i);
        *blue += *(s16 *)((u8 *)deltaBlue + i);
        r = ChannelValue(*red);
        g = ChannelValue(*green);
        b = ChannelValue(*blue);
        *color++ = r | (g << 5) | (b << 10);
        blue++;
        red++;
        i += 2;
        green++;
    } while ((s32)color <= (s32)&p->colors[15]);
    p->remaining--;
    if (!ReadMasked(0x02008000, &actor->flags2CC))
        BindPalette(actor, p);
    if (!p->remaining)
        return 1;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080FB744);
#endif
