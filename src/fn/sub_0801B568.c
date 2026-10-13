#include "global.h"
#define ResultMenu_InitializeLayer sub_0801B568
void *sub_0821A520(int, int);
void sub_082161B4(int, int, void *, int, int, int, void *);
void sub_0821656C(int, int, int, int, int);
int ResultMenu_InitializeLayer(void)
{
    void *resource = sub_0821A520(0xc091, 0xa597);
    int value = 2;
    sub_082161B4(0, 0, resource, 0, 0, 1, &value);
    sub_0821656C(0, 0, -4, 0, -24);
    return 0;
}
