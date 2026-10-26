#include "global.h"
#include "scene_vec.h"
#include "palette_set.h"
#include "render.h"
#define gPaletteBanks gPaletteBankData
struct SceneScriptFadeWidget {
    u8 unknown0[6];
    u16 flags;
};
struct SceneScriptFadeObject {
    u8 unknown0[0x18];
    u32 flags;
    u8 unknown1C[0x34 - 0x1C];
    struct SceneVec position;
    u8 unknown3C[0x4A - 0x3C];
    u16 paletteBank;
    u8 unknown4C[0x70 - 0x4C];
    u16 palette[16];
    u8 unknown90[4];
    struct SceneScriptFadeWidget widget;
    u8 unknown9C[0xEC - 0x9C];
    s16 state;
    s16 delay;
    u8 unknownF0[2];
    s16 fadeTimer;
    s16 holdTimer;
    s16 parameter;
    u32 pendingScript;
    u8 shadow[0x13C - 0xFC];
    u16 finished;
    u16 counter;
};
extern const s16 gUnk_086149C4[];
extern const s32 gUnk_0824F7A4[15];
void CpuSet(const void *, void *, u32);
void Sound_PlaySong(u32);
void ActorShadow_Disable(void *);
void SceneScriptObject_RunPendingScript(void *);
void *memcpy(void *, const void *, u32);
static inline s32 Next(s32 bank)
{
    return bank + 1;
}
static inline void ClearFlags(struct SceneScriptFadeWidget *w, s32 mask)
{
    w->flags &= mask;
}
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline s32 PaletteIndex(s32 state, s32 parameter)
{
    if (state <= 1) {
        if (parameter >= 0)
            return parameter + 0x26B;
        return 0x26B;
    }
    return ((state - 2) >> 1) + 0x26B;
}
void SceneScriptObject_UpdateFade(struct SceneScriptFadeObject *object)
{
    struct SceneScriptFadeWidget *widget;
    s32 songs[15];
    s32 index;
    s32 amount;
    s32 zero;
    u8 *dest;
    if (object->fadeTimer > 0 && (u16)--object->fadeTimer == 0) {
        index = PaletteIndex(object->state, object->parameter);
        object->paletteBank = index;
        CpuSet(gPaletteBanks + index * 32, object->palette, 0x04000008);
    }
    if (object->holdTimer > 0) {
        struct SceneScriptFadeWidget *w = &object->widget;
        w->flags = 4 | w->flags;
        object->holdTimer--;
        widget = w;
    } else {
        struct SceneScriptFadeWidget *w = &object->widget;
        ClearFlags(w, -5);
        widget = w;
    }
    switch (object->state) {
        case 12:
            object->position.y += Scale(gUnk_086149C4[*(u8 *)&object->counter] * 2);
            break;
        case 14:
            if (object->fadeTimer == 0) {
                u16 phase;
                s32 bank = 0x271;
                object->paletteBank = bank;
                phase = object->counter;
                if (phase & 0x40) {
                    s32 m = 0x7f;
                    m &= phase;
                    amount = 0x80 - m;
                } else {
                    s32 m = 0x7f;
                    m &= phase;
                    amount = m;
                }
                dest = gPaletteBanks + bank * 32;
                Palette_InterpolateBanks(object->palette, dest, gPaletteBanks + Next(bank) * 32,
                                         amount, 7);
            }
            break;
    }
    if (object->delay > 0)
        goto increment;
    zero = object->fadeTimer;
    if (zero != 0)
        goto increment;
    memcpy(songs, gUnk_0824F7A4, 0x3c);
    Sound_PlaySong(songs[object->state]);
    object->flags |= 1;
    if (object->state == 12)
        ActorShadow_Disable(object->shadow);
    object->finished = 1;
    object->counter = zero;
    widget->flags |= 4;
    if (object->pendingScript)
        SceneScriptObject_RunPendingScript(object);
    return;
increment:
    object->counter++;
}
