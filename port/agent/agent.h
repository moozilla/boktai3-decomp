/* Included first in every C snippet run by `play.py c` (port/agent/README.md).
 * A snippet is ordinary game C: the game's headers, globals at their GBA
 * addresses, and any game function called by name (declared or not; an
 * undeclared one takes and returns ints, as in C89). Its own data and stack
 * live at 0x02800000, outside everything the game uses. */
#ifndef BOKTAI_AGENT_H
#define BOKTAI_AGENT_H

/* printf to the command's output: %d %i %u %x %X %o %c %s %p %%, with -, 0,
 * width and an ignored l/h/hh. */
void print(const char *format, ...);
/* Hex dump of len bytes. */
void dump(const void *address, unsigned len);
/* Call any GBA code address with four word arguments (the result is r0). */
unsigned call_address(unsigned address, unsigned a0, unsigned a1, unsigned a2, unsigned a3);
/* Set the value `play.py c` prints as the result (also the return of snippet_main). */
#define RESULT(x) (agent_result = (unsigned)(x))
extern unsigned agent_result;

#endif
