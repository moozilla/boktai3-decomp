#include "global.h"
#include "scene_vec.h"
#include "render.h"
#include "engine.h"
#include "ram.h"
#include "monster_particle_random.h"

static inline void SetVector(struct SceneVec *v, s32 x, s32 y, s32 z, s32 pad)
{
    v->x = x;
    v->y = y;
    v->z = z;
    v->pad = pad;
}

struct SceneHudPrompt {
    u8 unk_00[0xA6D];
    u8 effectCountdown;
};
s32 Mod(s32, s32);
void *SpriteEffectController_Create(const struct SceneVec *, const struct SceneVec *, u32, u32, u32,
                                    u32, u32, u32, u32, u32, u32, u32, s32, s32, s32, s32, s32,
                                    s32);
void Hud_DrawAlternateMeterLabel(struct SceneHudPrompt *, s32, s32);
void StatDisplay_DrawGauge(struct SceneHudPrompt *, s32, s32);
void Hud_RefreshActivePanel(struct SceneHudPrompt *);
s32 PauseMenu_TryBeginEquipmentSelection(struct SceneHudPrompt *);
void Hud_BeginSavedBlend(struct SceneHudPrompt *);
void Sound_PlaySong(u32);

static inline s32 Random(const u16 *table, u32 *index)
{
    u16 sample;
    *index = (*index + 1) & 1023;
    PARTICLE_RANDOM_READ_SAMPLE(sample, table, *index);
    return sample;
}

// Tentative role: updates the HUD prompt before equipment selection.
void Hud_UpdateEquipmentPrompt(struct SceneHudPrompt *hud)
{
    struct SceneVec velocity;
    struct SceneVec position;
    u32 effectType = 0x1C1E;
    u32 resource = 0xD43F;
    if (hud->effectCountdown) {
        if (hud->effectCountdown > 30) {
            const u16 *table = gRandomTable;
            u32 *index = &gRandomIndex;
            s32 x, y, z;
            s32 px, py, life;
            struct SceneVec *v;
            s32 zero;
            x = ((s32)ParticleRandom_ReadDirectSample(table) >> 4) & 1;
            y = ((s32)ParticleRandom_ReadDirectSample(table) >> 4) & 1;
            z = ((s32)ParticleRandom_ReadDirectSample(table) >> 4) & 1;
            v = &velocity;
            zero = 0;
            v->x = x;
            v->y = y;
            v->z = z;
            if (((s32)ParticleRandom_ReadDirectSample(table) >> 4) & 1)
                v->x = v->x * -1;
            if (((s32)ParticleRandom_ReadDirectSample(table) >> 4) & 1)
                v->y = v->y * -1;
            px = Mod(Random(table, index), 80);
            py = Mod(Random(table, index), 8);
            SetVector(&position, px + 16, py + 16, zero, zero);
            life = ((Random(table, index) >> 4) & 7) + 1;
            SpriteEffectController_Create(&position, &velocity, effectType, resource, zero, 16, 17,
                                          1, zero, 1, zero, life, zero, zero, zero, zero, -4, -4);
        }
        if (hud->effectCountdown <= 39) {
            Hud_DrawAlternateMeterLabel(hud, 0, 3);
            StatDisplay_DrawGauge(hud, 3, 3);
        }
        hud->effectCountdown--;
        if (!hud->effectCountdown)
            hud->effectCountdown = 1;
    }
    if (!(gEnginePauseFlags & 1)) {
        Hud_RefreshActivePanel(hud);
        if (PauseMenu_TryBeginEquipmentSelection(hud))
            Sound_PlaySong(0x10C);
        else
            Hud_BeginSavedBlend(hud);
    }
}
