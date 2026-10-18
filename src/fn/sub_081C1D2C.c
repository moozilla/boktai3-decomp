#include "global.h"
void ControlPanel_ShowValueDisplay(void *);
void ControlPanel_ShowInputGauge(void *);
void ControlPanel_ShowInteractionDisplay(void *p)
{
    ControlPanel_ShowValueDisplay(p);
    ControlPanel_ShowInputGauge(p);
}
