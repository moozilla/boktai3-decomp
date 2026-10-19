#include "global.h"
#include "scene_vec.h"
#define SceneHazard_AdvanceToTarget sub_08180730
#define SceneHazard_CanAdvance      sub_081806F0
#define SceneObject_Deactivate      sub_081D31C8
#define gSceneSineTable             gUnk_086149C4
struct MovingSceneHazard {
    u8 unknown0[8];
    struct SceneVec position;
    void *owner;
    u8 motion[0x1c];
    struct SceneVec target;
    s16 active, damage;
};
void sub_0821FAD0(void *, void *);
s32 sub_081806F0(struct MovingSceneHazard *);
s32 Math_Atan2(s32, s32);
s32 AnimatedHazard_CreateProximity(struct SceneVec *, s32, s32, s32);
void sub_081D31C8(void *);
extern const s16 gUnk_086149C4[];
static inline s32 TruncateFixed(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
void sub_08180730(struct MovingSceneHazard *p)
{
    if (p->active != 0) {
        s32 arrived = 0;
        void *motion = p->motion;
        struct SceneVec *position = &p->position;
        sub_0821FAD0(motion, position);
        if (sub_081806F0(p)) {
            s32 dx = p->target.x - p->position.x;
            s32 dz = p->target.z - p->position.z;
            if (dx * dx + dz * dz <= 400) {
                p->position.x += dx;
                p->position.z += dz;
                arrived = 1;
            } else {
                u32 direction = Math_Atan2(dx, dz) & 0xe0;
                p->position.x += TruncateFixed(gUnk_086149C4[(direction + 64) & 255] << 3);
                p->position.z += TruncateFixed(gUnk_086149C4[direction] << 3);
            }
        }
        if (arrived == 1) {
            AnimatedHazard_CreateProximity(position, p->damage, 8, 30);
            p->active = 0;
        }
    } else {
        sub_081D31C8(p->owner);
    }
}
