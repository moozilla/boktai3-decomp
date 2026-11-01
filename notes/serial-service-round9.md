# Serial_ServiceInterrupt

The guarded C retains the first SIOMULTI0 halfword in r8 without a binding.
Each access to the four-packet ring now stays within its declared row. When
receiving and sending overlap, the interrupt delivers the oldest queued packet,
advances its read index and fills the freed slot with the new four words.

Stock agbcc produces 740 bytes and scores 89.7%, compared with 90.3% for the
pinned draft. Six realigned instructions differ after ignoring register choices.
The ROM reloads the read index after updating the saved packet count. The C
keeps it in a register.
