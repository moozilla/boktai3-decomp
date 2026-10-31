# PlayerClone_UpdatePool

Tags: #matching #player #compiler #round8

All 912 bytes match at 08200C94. The vector sum captures the address of the
second input's x component before reading it. In the area-collider projection
this produces the copy from r2 to r1 at 08200EFC. The x load then uses r1
directly, leaving the expected scratch register available for the attack code.
