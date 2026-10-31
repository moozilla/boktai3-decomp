#include "scene_actor_palette.h"
#include "scene_actor_placement.h"

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

static inline u32 Axe_ReadMaskedFlags(u32 mask, const u32 *field)
{
    return *field & mask;
}
static inline u8 Axe_TestFlags(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline s32 Axe_ChannelValue(s32 value)
{
    return value >= 0 ? value >> 8 : -(-value >> 8);
}
static inline void Axe_BindPalette(struct SceneActorRegistryActor *actor,
                                   struct AxePaletteTransition *p)
{
    ((struct SceneActorPaletteSequenceView *)actor->drawable)->palette = p->colors;
    Axe_SetAuxiliarySpriteResource(actor, p->colors);
}
static inline u32 Axe_ReadBindingFlags(u32 *const *flags, u32 mask)
{
    return **flags & mask;
}
static inline s16 *Axe_UpdateRedSlot(s16 *const *values, s16 *const *deltas, s32 index)
{
    s16 *slot = &(*values)[index];
    *slot += (*deltas)[index];
    return slot;
}
static inline void Axe_GetPaletteUpdateHandles(s16 **deltas, u32 **flags, s16 **values,
                                               struct SceneActorRegistryActor *actor,
                                               struct AxePaletteTransition *transition)
{
    *flags = &actor->flags2CC;
    *values = transition->red;
    *deltas = transition->deltaRed;
}
// confidence=low: advances the palette transition and binds its colors when the actor flags allow it.
u32 Axe_UpdatePaletteTransition(struct SceneActorRegistryActor *actor)
{
    struct AxePaletteContext *context;
    struct AxePaletteTransition *p;
    s16 *red;
    s16 *deltaRed;
    s32 i;
    u32 *actorFlags;
    if (SceneActorPlacement_IsUnavailable((void *)actor))
        return 0;
    context = actor->context;
    if (!Axe_TestFlags(Axe_ReadMaskedFlags(2, &context->flags), ~0))
        return 0;
    p = &context->transition;
    if (!p->remaining) {
        if (!Axe_ReadMaskedFlags(0x02008000, &actor->flags2CC))
            Axe_BindPalette(actor, p);
        return 1;
    }
    Axe_GetPaletteUpdateHandles(&deltaRed, &actorFlags, &red, actor, p);
    for (i = 0; i < 16; i++) {
        s32 r, g, b;
        s16 *redSlot;
        redSlot = Axe_UpdateRedSlot(&red, &deltaRed, i);
        p->green[i] += p->deltaGreen[i];
        p->blue[i] += p->deltaBlue[i];
        r = Axe_ChannelValue(*redSlot);
        g = Axe_ChannelValue(p->green[i]);
        b = Axe_ChannelValue(p->blue[i]);
        p->colors[i] = r | (g << 5) | (b << 10);
    }
    p->remaining--;
    if (!Axe_ReadBindingFlags(&actorFlags, 0x02008000))
        Axe_BindPalette(actor, p);
    if (!p->remaining)
        return 1;
    return 0;
}
