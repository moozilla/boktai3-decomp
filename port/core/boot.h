#ifndef BOKTAI_PORT_BOOT_H
#define BOKTAI_PORT_BOOT_H
/* AgbMain's reset/early boot, ending before Clock_CheckRtcDate. Returns -1 if
 * memory/ROM are not ready. The full entry uses this same tested prefix. */
int port_boot_early(void);
/* Copy the ROM's IWRAM payloads for data reads and diagnostics. Portable C code
 * is called through the resolver instead of executing these copied bytes. */
int port_boot_copy_iwram(void);
unsigned port_boot_call_count(void);
#endif
