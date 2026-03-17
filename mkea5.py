import sys, struct

if len(sys.argv) != 3:
    print("Usage: mkEA5.py BODY.BIN EA5FILE")
    sys.exit(1)

body = open(sys.argv[1], "rb").read()
if len(body) % 2 != 0:
    body += b'\x00'

entry = 0xA000
load  = 0xA000
length = len(body)

with open(sys.argv[2], "wb") as f:
    f.write(struct.pack(">H", 0x0000))       # EA5 version
    f.write(struct.pack(">H", entry))        # Entry point
    f.write(struct.pack(">H", load))         # Segment load address
    f.write(struct.pack(">H", length))       # Segment length
    f.write(body)                            # Segment data
    f.write(struct.pack(">H", 0xFFFF))       # End marker