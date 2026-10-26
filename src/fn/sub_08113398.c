#include "global.h"
#include "render.h"
#include "ram.h"
struct DebrisPos {
    s16 x, y, z, pad;
};
struct DebrisVisual {
    u8 unknown0[0x68];
    void *palette;
};
struct Debris {
    struct DebrisVisual *visual;
    struct DebrisPos position, velocity;
    u32 timer;
    u8 restitution, active, entered, visualType;
    void (*update)(struct Debris *);
};
struct DebrisPool {
    u8 task[0x18];
    struct Debris entries[64];
};
struct DebrisPool *SceneDebrisPool_EnsureSingleton(void);
void PanelSprite_Create(struct Debris *, u32, u32, u32, u32, u32, u32, u32);
void PanelKeyframeSprite_Create(struct Debris *, u32, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, s32);
void SceneDebris_UpdateAnimatedMotion(struct Debris *);
void SceneDebris_UpdateBouncingMotion(struct Debris *);
// confidence=low: claims a free debris slot, copies position/velocity, builds its panel sprite and picks the motion callback.
s32 sub_08113398(struct DebrisPos *pos, struct DebrisPos *velocity, u32 a2, u32 a3, u32 a4,
                 u32 keyframe, u32 a6, u32 a7, s32 palette, u32 timer, u32 motion, u32 a11, u32 a12,
                 u32 restitution)
{
    struct DebrisPool *pool;
    s32 i;
    if (!gDebrisPool) {
        pool = SceneDebrisPool_EnsureSingleton();
        if (!pool)
            return -1;
    } else
        pool = gDebrisPool;
    for (i = 0; i <= 63; i++) {
        if (!pool->entries[i].active) {
            struct Debris *e = &pool->entries[i];
            e->position.x = pos->x;
            e->position.y = pos->y;
            e->position.z = pos->z;
            if (!velocity) {
                e->velocity.x = 0;
                e->velocity.y = 0;
                e->velocity.z = 0;
            } else {
                e->velocity.x = velocity->x;
                e->velocity.y = velocity->y;
                e->velocity.z = velocity->z;
            }
            e->restitution = restitution;
            e->active = 1;
            e->entered = 1;
            e->timer = timer;
            if (!keyframe) {
                struct DebrisVisual *visual;
                PanelSprite_Create(e, a2, a3, a4, a6, a7, a11, a12);
                visual = e->visual;
                if (palette >= 0)
                    AnimData_SelectPaletteBank(visual, palette);
            } else {
                struct DebrisVisual *visual;
                PanelKeyframeSprite_Create(e, a2, a4, a11, a12);
                visual = e->visual;
                if (palette >= 0)
                    visual->palette = gPaletteBankData + palette * 32;
            }
            switch (motion) {
                case 0:
                    e->update = SceneDebris_UpdateAnimatedMotion;
                    break;
                case 1:
                    e->update = SceneDebris_UpdateBouncingMotion;
                    break;
                default:
                    e->update = SceneDebris_UpdateAnimatedMotion;
                    break;
            }
            return i;
        }
    }
    return -1;
}
