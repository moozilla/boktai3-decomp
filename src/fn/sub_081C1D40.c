#include "global.h"

void ControlPanel_HideValueDisplay(void *);
void ControlPanel_HideMultiplierSprites(void *);
void ControlPanel_HideInputGauge(void *);

void ControlPanel_HideInteractionDisplay(void *p)
{
    ControlPanel_HideValueDisplay(p);
    ControlPanel_HideMultiplierSprites(p);
    ControlPanel_HideInputGauge(p);
}
