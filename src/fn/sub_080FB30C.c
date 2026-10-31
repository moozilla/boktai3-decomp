#include "global.h"
#include "scene_actor_state.h"
#include "display_track.h"

struct AxeFlashState {
    u8 unk_00[0x908];
    u16 palette[16];
    u8 unk_928[0xE4C - 0x928];
    u32 flags;
    u8 unk_E50[0xECE - 0xE50];
    u8 timer;
    s8 level;
    u8 start;
    u8 stop;
    u8 unk_ED2[4];
    u8 reset;
};
struct AxeFlashDrawable {
    struct ContactSwarmSprite sprite;
    struct DisplayTrackGraphics graphics;
};

static inline u8 TestFlag(const u32 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}

/* Fade the attack palette, then restore the actor palette when the level reaches zero. */
void sub_080FB30C(struct SceneActorRegistryResetView *actor)
{
    struct AxeFlashState *state = actor->context;
    u32 *flags;
    if (state->start) {
        state->level = 0;
        state->timer = 0;
        {
            u32 bit = 0x80;
            state->flags |= bit;
        }
        state->start = 0;
    }
    {
        u8 stop = state->stop;
        flags = &state->flags;
        if (stop) {
            {
                u32 bit = 0x100;
                *flags |= bit;
            }
            *flags &= ~0x80;
            state->level = 3;
            state->timer = 0;
            state->stop = 0;
        }
    }
    if (state->reset) {
        {
            u32 bit = 0x100;
            *flags |= bit;
        }
        *flags &= ~0x80;
        {
            s32 lv = state->level;
            if (lv > 3)
                lv = 3;
            else if (lv < 0)
                lv = 0;
            state->level = lv;
        }
        state->timer = 0;
        state->reset = 0;
    }
    {
        u32 mask = 0x180;
        if (TestFlag(flags, mask)) {
            ((struct AxeFlashDrawable *)actor->drawable)->graphics.defaultPalette = 0x13F;
            ((struct AxeFlashDrawable *)actor->drawable)->graphics.palette = state->palette;
            switch (state->level) {
                case 0:
                    state->palette[1] = 0;
                    break;
                case 1:
                    state->palette[1] = 16;
                    break;
                case 2:
                    state->palette[1] = 24;
                    break;
                case 3:
                    state->palette[1] = 31;
                    break;
            }
            if (++state->timer > 9) {
                if (!TestFlag(flags, 0x80)) {
                    if (--state->level <= 0) {
                        void *drawable = &((struct AxeFlashDrawable *)actor->drawable)->graphics;
                        u16 *palette = &actor->basePaletteBank;
                        u16 *offset = &actor->paletteOffset;
                        AnimData_SelectPaletteBank(drawable, *palette + *offset);
                        {
                            u32 clr = ~0x100;
                            *flags &= clr;
                        }
                        return;
                    }
                } else {
                    if (state->level < 3)
                        state->level++;
                }
                state->timer = 0;
            }
        }
    }
}
