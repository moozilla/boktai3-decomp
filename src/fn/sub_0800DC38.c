#include "scanline_presentation.h"
void sub_082194D4(void *);
void sub_0821910C(void);
int ScanlinePresentation_Destroy(struct ScanlinePresentation *p)
{
    int i;
    struct ScanlineLayer *layer;
    sub_082194D4(p);
    sub_0821910C();
    Anim_Remove(&p->layers[0]);
    Anim_Remove(&p->layers[1]);
    Anim_Remove(&p->layers[2]);
    Anim_Remove(&p->layers[3]);
    layer = &p->layers[4];
    for (i = 1; i >= 0; i--) {
        Anim_Remove(layer);
        layer++;
    }
    return 0;
}
