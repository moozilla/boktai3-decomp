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
void sub_082195E0(Layer *);
#define ScanlinePresentation_Destroy sub_0800DC38
int ScanlinePresentation_Destroy(Presentation *p)
{
    int i;
    Layer *layer;
    sub_082194D4(p);
    sub_0821910C();
    sub_082195E0(&p->first);
    sub_082195E0(&p->second);
    sub_082195E0(&p->third);
    sub_082195E0(&p->fourth);
    layer = p->tail;
    for (i = 1; i >= 0; i--) {
        sub_082195E0(layer);
        layer++;
    }
    return 0;
}
