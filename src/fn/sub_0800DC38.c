#include "global.h"
typedef struct {
    u8 bytes[96];
} Layer;
typedef struct {
    u8 pad[0x2EC];
    Layer first, second, third, fourth, tail[2];
} Presentation;
void sub_082194D4(void *);
void sub_0821910C(void);
void Anim_Remove(Layer *);
int ScanlinePresentation_Destroy(Presentation *p)
{
    int i;
    Layer *layer;
    sub_082194D4(p);
    sub_0821910C();
    Anim_Remove(&p->first);
    Anim_Remove(&p->second);
    Anim_Remove(&p->third);
    Anim_Remove(&p->fourth);
    layer = p->tail;
    for (i = 1; i >= 0; i--) {
        Anim_Remove(layer);
        layer++;
    }
    return 0;
}
