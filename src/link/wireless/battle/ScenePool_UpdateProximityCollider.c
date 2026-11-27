#include "global.h"
void Character_GetHeightAdjustedPosition(void *, s16 *);
void SpatialIndex_SetPosition(void *, int, int, int);
void ScenePool_UpdateProximityCollider(u8 *p)
{
    s16 vector[3];
    s16 *q = vector;
    Character_GetHeightAdjustedPosition(p, vector);
    SpatialIndex_SetPosition(p + 0x860, vector[0], q[1], q[2]);
}
