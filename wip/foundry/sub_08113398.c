#define SpriteEffect_Create sub_08113398
#define SpriteEffect_CreateManager sub_0811334C
#define gSpriteEffectManager gUnk_020004C4
#include "global.h"

struct SpriteEffect {
    void *sprite;
    s16 position[3];
    u8 padA[2];
    s16 velocity[3];
    u8 pad12[2];
    u32 value;
    u8 mode, active, enabled, pad1B;
    void (*update)(void);
};
struct SpriteEffectManager { u8 task[0x18]; struct SpriteEffect effects[64]; };
extern struct SpriteEffectManager *gUnk_020004C4;
extern u8 *gUnk_030042E4;
struct SpriteEffectManager *sub_0811334C(void);
void sub_08112CC8(struct SpriteEffect *, u32, u32, u32, u32, u32, u32, u32);
void sub_08112D6C(struct SpriteEffect *, u32, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, u32);
void sub_08113104(void);
void sub_08112EA8(void);
s32 sub_08113398(s16 *pos, s16 *vel, u32 resource, u32 a, u32 b, u32 kind, u32 c, u32 d, s32 palette, u32 value, s32 update, u32 e, u32 f, u32 mode)
{
    struct SpriteEffectManager *manager;
    s32 i;
    s16 *position;
    manager = gUnk_020004C4;
    if (!manager) { manager = sub_0811334C(); if (!manager) return -1; }
    for (i = 0, position = manager->effects[0].position; i < 64; i++, position += 16) {
        struct SpriteEffect *effect;
        if (!*((u8 *)position + 0x15)) {
            effect = &manager->effects[i];
            position[0] = pos[0];
            position[1] = pos[1];
            position[2] = pos[2];
            if (!vel) { position[4] = 0; position[5] = 0; position[6] = 0; }
            else { effect->velocity[0] = vel[0]; effect->velocity[1] = vel[1]; effect->velocity[2] = vel[2]; }
            effect->mode = mode; effect->active = 1; effect->enabled = 1;
            effect->value = value;
            if (kind == 0) {
                sub_08112CC8(effect, resource, a, b, c, d, e, f);
                { void *sprite = effect->sprite; if (palette >= 0) AnimData_SelectPaletteBank(sprite, palette); }
            } else {
                sub_08112D6C(effect, resource, b, e, f);
                { void *sprite = effect->sprite; if (palette >= 0) *(u8 **)((u8 *)sprite + 0x68) = gUnk_030042E4 + (palette << 5); }
            }
            switch (update) { case 0: effect->update = sub_08113104; break; case 1: effect->update = sub_08112EA8; break; default: effect->update = sub_08113104; break; }
            return i;
        }
    }
    return -1;
}
