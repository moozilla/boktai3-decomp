#ifndef BOKTAI_PORT_HW_BIOS_SHIM_H
#define BOKTAI_PORT_HW_BIOS_SHIM_H

/* Force-include for port builds, after the core's general C compatibility
 * shim. The SDK's include guard prevents it from restoring ARM inline asm. */
#include "gba/types.h"
#include "gba/syscall.h"
#include "hw.h"
#undef SystemCall
#define SystemCall(n) hw_system_call(n)

#endif
