#!/bin/bash

# create base ROM image
./make_padding.pl > rom

# 64k image
dd if=../../monitor/src/monitor.bin of=rom bs=1k seek=0 conv=sparse,nocreat,notrunc

# 64k image
dd if=../../basic/src/basic.bin of=rom bs=1k seek=64 conv=sparse,nocreat,notrunc

# 64k image
dd if=../../stub/src/stub.bin of=rom bs=1k seek=128 conv=sparse,nocreat,notrunc

# 16k image
dd if=../../misc/src/out16.bin of=rom bs=1k seek=192 conv=sparse,nocreat,notrunc

# split the ROM image ready for burning
../../utils/split/split --file=rom
