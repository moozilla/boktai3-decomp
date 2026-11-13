#ifndef BOKTAI_PORT_COMPAT_H
#define BOKTAI_PORT_COMPAT_H
/* Forced include for the portable build only. The ROM build never sees this.
 * Erase historical register pins/barriers and assembler includes. Hardware
 * assembly is replaced explicitly by port functions; it is never executable. */
#define asm(...)
#define __asm__(...)
#define register
void port_system_call(unsigned int number);
#endif
