// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
extern s16 gUnk_086149C4[];
s32 Div(s32, s32);
void Sound_PlaySong(u32);
void MusicSlots_StopAllAndRetainResumable(void);
void HealthRestoreEffect_StartParticleBurst(u8 *);
void HealthRestoreEffect_UpdateShake(u8 *);
struct HealthRestoreEntry {
    u8 unk00[0x24];
    u32 spriteFlags;
    u8 unk28[0x14];
    u16 screenX, screenY;
    u8 unk40[0x10];
    u8 scale[2];
    u8 unk52[0x2A];
    s16 targetX, targetY;
    u8 unk80[4];
    s16 startX, startY;
    u8 unk88[0x1E8];
    u16 timer, unk272;
    void (*callback)(u8 *);
};
static inline s32 scale12(s32 v)
{
    if (v >= 0)
        v = v >> 12;
    else
        v = -((-v) >> 12);
    return v;
}
// confidence=low: the 16-frame entry of the health restore effect. Stops the music, then slides
// the sprite from its start to its target position with a sine bob; afterwards it starts the
// shake.
void sub_08234DC8(struct HealthRestoreEntry *p)
{
    u16 *timer;
    s32 y;
    register u32 scaleTimer asm("r1");
    u32 value;
    if (p->timer == 0) {
        MusicSlots_StopAllAndRetainResumable();
        {
            s32 four = 4;
            gEnginePauseFlags |= four;
        }
    } else if (p->timer == 1) {
        Sound_PlaySong(0xE6);
        p->spriteFlags &= ~1;
    }
    timer = &p->timer;
    value = *timer + 1;
    scaleTimer = value;
    *timer = value;
    if ((u16)value > 15) {
        p->spriteFlags &= ~2;
        HealthRestoreEffect_StartParticleBurst((u8 *)p);
        p->callback = HealthRestoreEffect_UpdateShake;
        *timer = 0;
        Sound_PlaySong(0x284);
        return;
    }
    {
        s32 v = scaleTimer + 0x22;
        p->scale[1] = v;
        p->scale[0] = v;
    }
    p->screenX = Div((p->targetX * (*timer)) + (p->startX * (16 - (*timer))), 16);
    y = Div((p->targetY * (*timer)) + (p->startY * (16 - (*timer))), 16);
    p->screenY = y - scale12(gUnk_086149C4[Div((*timer) << 7, 20) & 0xFF] << 4);
}
