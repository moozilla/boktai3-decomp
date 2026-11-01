#include "scene_departure_display.h"
#include "ram.h"
#include "palette_set.h"

extern u16 gUnk_03005040[4][16];
extern const u16 gUnk_086141BC[];
void sub_08113398(struct ContactPackedVector *, struct ContactPackedVector *, u32, u32, s32, s32,
                  s32, s32, s32, s32, s32, s32, s32, s32);

static inline u32 RandomValue(void)
{
    u32 index = (gEffectRandomTableIndex + 1) & 1023;
    gEffectRandomTableIndex = index;
    return gUnk_086141BC[index];
}

void SceneActors_UpdatePaletteDispersal(struct DepartureActor *input)
{
    struct DepartureActor *actor = input;
    s32 i;
    if (input->entered) {
        input->entered = 0;
        input->unknown394 |= 0x20000;
        input->fadeFrame = 0;
        input->fadeShift = 6;
        input->motionState = 0;
        input->dispersalTimer = 0;
    }
    switch (actor->motionState) {
        case 0: {
            u16 *from, *to;
            actor->fadeFrame++;
            {
                u8 *left = gPaletteBankData + 0x60;
                u8 *right = gPaletteBankData + 0x3EC0;
                u16 *dst = actor->foregroundPalette;
                Palette_InterpolateBanks(dst, left, right, actor->fadeFrame, actor->fadeShift);
            }
            actor->foregroundGraphics.paletteData = actor->foregroundPalette;
            {
                u8 *left = gPaletteBankData + 0x40;
                u8 *right = gPaletteBankData + 0x3EA0;
                u16 *dst = actor->accentPalette;
                Palette_InterpolateBanks(dst, left, right, actor->fadeFrame, actor->fadeShift);
            }
            actor->accentGraphics.paletteData = actor->accentPalette;
            i = 0;
            to = actor->fadeTo[0];
            from = actor->fadeFrom[0];
            do {
                Palette_InterpolateBanks(gUnk_03005040[i], from, to, actor->fadeFrame,
                                         actor->fadeShift);
                to += 16;
                from += 16;
            } while (++i <= 3);
            if (actor->fadeFrame > (1 << actor->fadeShift)) {
                actor->foreground.mode = 1;
                actor->accent.mode = 1;
                actor->motionState = 1;
            } else {
                actor->foreground.mode = 0;
                actor->accent.mode = 0;
                if (actor->fadeFrame > (1 << (actor->fadeShift - 1))) {
                    if (actor->fadeFrame & 1) {
                        actor->foreground.mode = 1;
                        actor->accent.mode = 1;
                    }
                    if (actor->fadeFrame > (1 << (actor->fadeShift - 2)) &&
                        Mod(actor->fadeFrame, 3) == 0) {
                        actor->foreground.mode = 1;
                        actor->accent.mode = 1;
                    }
                }
            }
            break;
        }
        case 1:
            if (++actor->dispersalTimer > 49)
                actor->motionState = 2;
            break;
        case 2: {
            struct ContactPackedVector position, velocity;
            for (i = 0; i < 5; i++) {
                position.x = (actor->position.x >> 12) + 50;
                position.y = 138;
                position.z = 50;
                velocity.x = (RandomValue() & 3) - 1;
                velocity.y = -(RandomValue() & 1);
                velocity.z = 0;
                sub_08113398(&position, &velocity, actor->effectResource, actor->effectGraphics, 5,
                             0, 0, 0, 2, 42, 0, 512, 1, 0);
                actor->emitted++;
            }
            if (actor->emitted > 23)
                actor->motionState = 3;
            break;
        }
        case 3:
            actor->finished = 1;
            break;
    }
    ActorDisplay_UpdateScreenAnchor(input);
    ActorDisplay_UpdateForegroundSprite(input);
    ActorDisplay_UpdateAccentSprite(input);
}
