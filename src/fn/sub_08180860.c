#define ProximityVolume_SetPosition     sub_08180860
#define ProximityVolume_TranslateBounds sub_0821FD3C
#define PositionReference_SetSource FrostEmitter_StartEmission
#include "global.h"
#include "contact_actor.h"
struct ProximityVolume {
    u8 unknown0[8];
    struct ContactPosition position;
    void *reference;
    u8 bounds[0x24];
    u16 latch;
};
void sub_0821FD3C(void *, struct ContactPosition *);
void FrostEmitter_StartEmission(void *, struct ContactPosition *);
void sub_08180860(struct ProximityVolume *volume, struct ContactPosition *position)
{
    struct ContactPosition *stored = &volume->position;
    *stored = *position;
    volume->latch = 1;
    sub_0821FD3C(volume->bounds, stored);
    FrostEmitter_StartEmission(volume->reference, stored);
}
