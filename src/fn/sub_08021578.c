#include "global.h"
#define SceneActors_Destroy sub_08021578
void sub_0815F738(void *);
void sub_08020D08(void *);
void sub_08225938(void *);
void sub_080213FC(int);
int SceneActors_Destroy(u8 *p)
{
    sub_0815F738(p + 0x15c);
    sub_08020D08(p + 0x118);
    sub_08225938(p + 0x48);
    sub_080213FC(p[0x9b]);
    return 0;
}
