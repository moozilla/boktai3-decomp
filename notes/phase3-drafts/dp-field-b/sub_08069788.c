#include "bouncing_object.h"
#define BouncingObject_IsActive sub_08069788
u32 BouncingObject_IsActive(struct BouncingObject *input) {
if (input) { if (input->index >= 0) { if (input->timer) { if (input->state != 2) return 1; } } } return 0;
}
