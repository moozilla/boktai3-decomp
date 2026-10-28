#include "global.h"
#include "ram.h"
extern void Serial_ServiceInterrupt(void);
void Serial_EnableInterrupt(void)
{
    *(volatile u16 *)0x04000208 = 0;
    *(volatile u16 *)0x04000200 &= 0xFF7F;
    gSerialInterruptState[1] = Serial_ServiceInterrupt;
    *(volatile u16 *)0x04000208 = 1;
    *(volatile u16 *)0x04000202 = 0x80;
    *(volatile u16 *)0x04000128 |= 0x4000;
    *(volatile u16 *)0x04000208 = 0;
    *(volatile u16 *)0x04000200 |= 0x80;
    *(volatile u16 *)0x04000208 = 1;
}
