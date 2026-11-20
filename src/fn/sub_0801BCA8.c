#include "result_tutorial.h"
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void *AnimData_FindEntry(int);
void ParticleSprite_Initialize(void *, void *, int);
int ResultMenu_CreateGraphics(struct ResultMenu *p)
{
    struct ContactPosition position;
    struct ContactPosition *pos = &position;
    struct SceneInputWidget *widget;
    struct ResultTutorialMarker *marker;
    int i;
    struct ContactPosition *saved;
    int clear;
    ZeroFill8(pos);
    saved = pos;
    clear = 0;
    widget = p->widgets;
    i = 14;
    do {
        Widget_InitLinked(widget, &p->primaryResource, 0xb9, 0x11, clear, clear, 60, saved);
        widget++;
        i--;
    } while (i >= 0);
    {
        void *resource = AnimData_FindEntry(0x1c1a);
        u8 *a = &p->hideLevel;
        u8 *b = &p->hideTimer;
        marker = p->markers;
        i = 3;
        do {
            ParticleSprite_Initialize(marker, resource, 0x11);
            marker++;
            i--;
        } while (i >= 0);
        *a = 0;
        *b = 0;
    }
}
