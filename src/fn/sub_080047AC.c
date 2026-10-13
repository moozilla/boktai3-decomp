#include "global.h"
#include "scene_object.h"

void sub_08214514(void *);
void sub_0821D6D0(void *);
void sub_0821FE6C(void *);
void sub_08003C10(void *);
void sub_08225938(void *);

/* Release everything an object holds (draw, cell reference, collider, body) and free its slot. */
u32 sub_080047AC(struct SceneObject *object)
{
    sub_08214514(&object->draw);
    sub_0821D6D0(&object->cellRef);
    sub_0821FE6C(&object->collider);
    if ((object->flags & 2) == 0)
        sub_08003C10(&object->body);
    sub_08225938(&object->body);
    return object->active = 0;
}
