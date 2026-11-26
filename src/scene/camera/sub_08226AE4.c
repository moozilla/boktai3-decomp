#include "scene/camera/scene_camera_controller.h"
#include "global.h"
#include "engine/video/render.h"
#include "engine/ram.h"

struct Rect {
    s16 x, size, y;
};
void Camera_ApplyPositionBounds(struct SceneCameraController *, void *);
/* Camera mode 1 with limits centered on two ranges {center, size, center2}:
   center - size / 2 (+0x8E..+0x94), then Camera_ApplyPositionBounds (tentative). */
void Camera_SetCenteredBounds(struct Rect *a, struct Rect *b)
{
    struct SceneCameraController *c = (void *)gActiveCamera;
    if (c) {
        c->minBounds[0] = a->x - (a->size >> 1);
        c->minBounds[1] = a->y - (a->size >> 1);
        c->maxBounds[0] = b->x - (b->size >> 1);
        c->maxBounds[1] = b->y - (b->size >> 1);
        c->boundsMode = 1;
        Camera_ApplyPositionBounds(c, &gCamera.position);
    }
}
