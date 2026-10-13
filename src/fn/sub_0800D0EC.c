#include "falling_objects.h"
void FallingObject_UpdateFalling(struct FallingObject *object)
{
    if (object->entered) {
        object->entered = 0;
        if (object->animated)
            Anim_SetSequence(&object->anim, gUnk_0200046C->animation, 3, 0, 0);
    }
    object->position.y -= object->speed;
    if (object->position.y <= object->ground) {
        object->position.y = object->ground;
        if (object->type == 2) {
            if (object->flags & 4)
                Sound_PlaySong(669);
            else
                Sound_PlaySong(511);
        } else
            Sound_PlaySong(326);
        {
            u32 one = 1;
            u32 zero = 0;
            u32 frame;
            object->state = one;
            frame = 0;
            object->timer = zero;
            object->entered = one;
            object->frame = frame;
            object->frameTimer = frame;
        }
    }
    if (object->type > 1) {
        object->frameTimer++;
        if (object->frameTimer == 5) {
            object->frame++;
            if (object->frame > 2)
                object->frame = 0;
            object->frameTimer = 0;
        }
        FallingObject_SetFrame(&object->sprite, object->frame);
    }
    if (object->collider)
        Collider_SetPosition(object->collider, &object->position, 0);
    if (object->attack) {
        Collider_SetPosition(object->attack, &object->position, 0);
        Collider_Refresh(object->attack);
    }
}
