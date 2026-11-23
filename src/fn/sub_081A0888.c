#include "global.h"
#include "hresvelgr_travel.h"
#include "scene_vec_polar.h"

static inline void AddVector(struct SceneVec *a, struct SceneVec *b)
{
    b->x += a->x;
    b->y += a->y;
    b->z += a->z;
}

void Hresvelgr_UpdateAlternatingOrbit(struct HresvelgrTravelActor *p, int tick)
{
    Hresvelgr_SetFlags(p, 0x1c);
    if (p->changed) {
        p->changed = 0;
        p->side = (p->side + 1) & 1;
    }
    if (tick > 119) {
        SceneVec_SetPolarXZ(&p->position, (p->side << 6) | 128, 1280);
        AddVector(p->origin, &p->position);
        p->position.y = p->origin->y;
        p->finished = 1;
    }
}
