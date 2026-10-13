#include "animated_hazard.h"

struct AnimatedHazardManager *sub_08012040(void);
void *sub_080126DC(void);
struct AnimatedHazard *sub_08012728(struct AnimatedHazardManager *);
void sub_0801204C(void *, struct AnimatedHazard *, u32);
static inline u8 *cell(int i)
{
    u8 *p = sub_0821D64C(i, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + 12 + i * 4;
}
int AnimatedHazard_Create(struct SceneVec *source, int damage, int hold)
{
    struct AnimatedHazardManager *m = sub_08012040();
    struct AnimatedHazard *p;
    struct SceneVec *pos;
    void *effect;
    u8 *c;
    int height, slope;
    int zero;
    if (!m) {
        m = sub_080126DC();
        if (!m)
            return -1;
    }
    p = sub_08012728(m);
    if (!p)
        return -1;
    p->sprite.position = *source;
    {
        struct SceneVec *ground = &p->sprite.position;
        int x = ground->x >> 8, z = ground->z >> 8, index;
        pos = ground;
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            index = 0;
        else
            index = gUnk_030052F4->rows[z] + x;
        c = cell(index);
        slope = *c >> 4;
        height = (*c & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)ground->z;
                break;
            case 2:
                height -= (u8)ground->x;
                break;
        }
    }
    {
        s16 *h = &p->sprite.position.y;
        zero = 0;
        *h = height;
    }
    effect = p->collider;
    Collider_SetPosition(effect, pos, 0);
    sub_0821FF50(effect, (u16)damage, 50);
    sub_0801204C(m, p, 2);
    p->timer = zero;
    p->hold = hold;
    p->active = 1;
    return 0;
}
