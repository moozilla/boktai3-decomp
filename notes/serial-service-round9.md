# Serial_ServiceInterrupt

All 740 bytes match with stock agbcc. The interrupt samples all four volatile
multiplayer registers and reads SIOMULTI0 once before checking transfer status.
Both enqueue paths calculate the four-entry ring tail from the read index and
the occupied count.

With a full receive buffer pending, the handler queues the new words. When the
receive buffer is available and earlier packets are waiting, it delivers the
oldest one and replaces it with the new packet. A one-halfword snapshot keeps
the queue depth across that replacement and gives agbcc the original stack
store and reload.

Tags: #matching #round9b #serial
