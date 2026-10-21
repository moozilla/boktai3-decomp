// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
#define F(p, o, T) (*(T *)((u8 *)(p) + (o)))
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
u8 *Path_GetPointData(u8 *);
void Spider_SelectRandomEntry(void *);
struct Vec2BF {
    s32 x : 16;
    s32 pad0 : 16;
    s32 z : 16;
    s32 pad1 : 16;
};
// confidence=low: Spider picks the route point ahead of its heading toward the current target.
void sub_080A4798(struct SceneActorRegistryResetView *a)
{
    u8 *nav = (u8 *)a + 0x3D4;
    u8 *r = (u8 *)a + 0x448;
    struct Vec2BF heading, delta;
    u32 index = 0;
    u8 *p;
    s32 i;
    u32 angle;
    struct ContactPosition *dest = F(a, 0xDC, void *);
    angle = (u8)Math_Atan2(F(dest, 0xC, s16) - a->position.x, F(dest, 0x10, s16) - a->position.z);
    heading.x = (u32)(gUnk_086149C4[(angle + 64) & 255] * 160) >> 16;
    heading.z = (u32)(gUnk_086149C4[angle] * 160) >> 16;
    p = Path_GetPointData(F(r, 4, u8 *));
    for (i = 0; i < *F(r, 4, u8 *); i++) {
        delta.x = F(p, 0, u16) - (u16)a->position.x;
        delta.z = F(p, 2, u16) - (u16)a->position.z;
        if (heading.x * delta.x + heading.z * delta.z >= 0) {
            p += 8;
        } else if (F(p, 0, u32) != **F(nav, 0x7C, u32 **)) {
            index = (u8)i;
            F(r, 2, u8) = index;
            F(r, 3, u8) = 0;
            F(r, 8, u8 *) = p;
            return;
        }
    }
    Spider_SelectRandomEntry(nav + 0x74);
}
