#include "named_object.h"

void NamedActor_PlayMovementSound(struct NamedObject *object)
{
    if (!(object->field_E0 & 4)) {
        switch (object->row) {
            case 3:
                if (!(object->field_E0 & 0x20))
                    goto b;
            case 0:
                Sound_PlaySong(0x158);
                break;
            b:
            case 1:
            case 2:
            case 4:
                Sound_PlaySong(0x261);
                break;
            default:
                Sound_PlaySong(0x261);
                break;
        }
    }
}
