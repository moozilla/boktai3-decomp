#include "scene/camera/scene_camera_controller.h"
#include "global.h"
#include "engine/video/render.h"
#include "engine/ram.h"
#include "scene/camera/scene_camera_vector_layout.h"

void Camera_ApplyPositionBounds(struct SceneCameraController *, void *);
/* Camera mode 2 with limits from two ranges {start, ?, end} (+0x8E..+0x94),
   then Camera_ApplyPositionBounds (tentative). */
void Camera_SetBounds(struct SceneCameraVector *a, struct SceneCameraVector *b)
{
    struct SceneCameraController *c = (void *)gActiveCamera;
    if (c) {
        c->minBounds[0] = a->z - a->x;
        c->minBounds[1] = a->z + a->x - a->y;
        c->maxBounds[0] = b->z - b->x;
        c->maxBounds[1] = b->z + b->x - b->y;
        c->boundsMode = 2;
        Camera_ApplyPositionBounds(c, &gCamera.position);
    }
}
