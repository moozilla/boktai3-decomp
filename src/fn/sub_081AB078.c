#include "global.h"
#include "render.h"
// confidence=low: palette transition accompanies the leap motion track.
// MATCHING_CONCESSION: register-pinning
struct VanargandPart {
    u8 pad0[0x10];
    u16 state;
};
struct Vanargand {
    u8 pad0[0x54];
    u8 motionData[0x9C - 0x54];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[0xAF - 0xA8];
    u8 finished, entered, pendingAnimation, ready;
    u8 padB3[0x18C - 0xB3];
    u8 animation[0x60];
    u8 drawable[0x3FC - 0x1EC];
    struct VanargandPart part;
    u8 pad410[0x500 - 0x410];
    u16 partActive;
    u8 pad502[0x534 - 0x502];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x56C - 0x55E];
    void *attachment;
    u8 pad570[0x5B0 - 0x570];
    u16 delay;
    u8 pad5B2[0x5B8 - 0x5B2];
    u16 field5B8, field5BA;
    u8 pad5BC[0x718 - 0x5BC];
    u8 palette[0x918 - 0x718];
    u8 fadeA[0xB18 - 0x918];
    u8 fadeB[0xD18 - 0xB18];
    u8 fadeC[0xF24 - 0xD18];
    u32 variant;
    u8 padF28[0xF34 - 0xF28];
    s32 transition;
    u8 padF38[0xF44 - 0xF38];
    void *afterimages;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
void Anim_SetFrameDurationScale(void *, u32);
static inline u8 complete(struct Vanargand *p)
{
    u8 result = 0;
    if (p->trackState == 2)
        result = 1;
    return result;
}

static inline void initialize(void *track, void *data, s16 x, s16 y, s16 z, s16 *target, u32 mode,
                              s16 *origin)
{
    s16 *q = origin;
    *q = x;
    origin[1] = y;
    origin[2] = z;
    MotionTrack_Initialize(track, data, origin, target, mode);
}

void Sound_PlaySong(u32);
void VanargandAfterimagePool_Spawn(void *, void *);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
void Vanargand_ResetPaletteTransition(struct Vanargand *);
void CpuSet(const void *, void *, u32);
#define VANARGAND_PALETTE_ADDR 0x03004FC0 /* gBgPaletteBuffer */

static inline s32 Scale8(s32 n)
{
    return n >= 0 ? n >> 8 : -((-n) >> 8);
}
#define LERP(dst, t, from, to, tag)                                                                \
    {                                                                                              \
        if ((t) > 0)                                                                               \
            goto tag##Upper;                                                                       \
        dst = (from);                                                                              \
        goto tag##Store;                                                                           \
        tag##Upper:                                                                                \
        {                                                                                          \
            s32 limit = 3;                                                                         \
            if ((t) < limit)                                                                       \
                goto tag##Interp;                                                                  \
        }                                                                                          \
        dst = (to);                                                                                \
        goto tag##Store;                                                                           \
        tag##Interp:                                                                               \
        {                                                                                          \
            s32 k = ((to) - (from)) << 8;                                                          \
            s32 v = (t) * k;                                                                       \
            s32 d = 3;                                                                             \
            s32 result;                                                                            \
            v = v / d;                                                                             \
            if (v >= 0)                                                                            \
                result = v >> 8;                                                                   \
            else                                                                                   \
                result = -(-v >> 8);                                                               \
            dst = result;                                                                          \
            dst += (from);                                                                         \
        }                                                                                          \
        tag##Store :;                                                                              \
    }

void Vanargand_UpdatePaletteLeap(struct Vanargand *p, s32 frame)
{
    s32 savedFrame = frame;
    struct Vanargand *self = p;
    u8 *animation;
    s16 first[4];
    animation = p->animation;
    {
        u32 mask = 4;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 1);
    if (p->pendingAnimation) {
        s32 kind;
        p->pendingAnimation = 0;
        kind = 0x6F;
        if (!p->variant)
            kind = 0x7F;
        switch (kind) {
            case 0x6F: {
                s32 t;
                s32 r, g, b;

                CpuSet(gPaletteBankData + 0xDE0, p->palette, 0x04000020);
                CpuSet(p->fadeA, gBgPaletteBuffer, 0x04000068);
                t = p->transition;
                LERP(r, t, 7, 0x13, r1)
                LERP(g, t, 0, 0x12, g1)
                LERP(b, t, 0, 0x13, b1)
                r |= g << 5;
                r |= b << 10;
                {
                    u16 color = r;
                    u16 *pal = gBgPaletteBuffer;
                    pal[0x56 / 2] = color;
                    pal[0x10C / 2] = color;
                }
                LERP(r, t, 0x15, 0x18, r2)
                LERP(g, t, 0, 0x1D, g2)
                LERP(b, t, 0, 0x1F, b2)
                r |= g << 5;
                r |= b << 10;
                {
                    u16 *pal;
                    u32 color = (u16)r;
                    // Keep the second palette address independently rematerializable.
                    pal = (u16 *)VANARGAND_PALETTE_ADDR;
                    asm("" : : "r"(pal), "r"(color));
                    pal[0x5A / 2] = color;
                    pal[0x10E / 2] = color;
                }
                break;
            }
            case 0x82:
                CpuSet(gPaletteBankData + 0x1040, p->palette, 0x04000020);
                CpuSet(p->fadeB, gBgPaletteBuffer, 0x04000068);
                break;
            case 0x7F:
                CpuSet(gPaletteBankData + 0xFE0, p->palette, 0x04000018);
                CpuSet(p->fadeC, gBgPaletteBuffer, 0x04000068);
                break;
            default:
                CpuSet(gPaletteBankData + 0x3240, self->palette, 0x04000008);
                break;
        }
        Vanargand_ResetPaletteTransition(self);
        SceneActors_StartUnmirroredAnimation(p, animation, animation + 0x60, 4, 3, 4);
        initialize(self->track, p->motionData, 0x280, 0x300, 0x280, 0, 4, first);
        Sound_PlaySong(0x13E);
        self->delay = 8;
        {
            u8 *attachment = self->attachment;
            u8 alpha = 0xA0;
            attachment[0x21F] = alpha;
            ((u8 *)self->attachment)[0x43B] = alpha;
        }
        PairedSceneActor_ApplyToSelectedHalves(self->attachment, 3, 0x16);
    }
    MotionTrack_Update(self->track);
    if (complete(self) && savedFrame > 0x3C) {
        {
            struct VanargandPart *part = &self->part;
            part->state = 7;
        }
        self->partActive = 0;
        p->finished = 1;
        return;
    }
    VanargandAfterimagePool_Spawn(self->afterimages, p->motionData);
    {
        u32 mask = 5;
        p->flags |= mask;
    }
}
