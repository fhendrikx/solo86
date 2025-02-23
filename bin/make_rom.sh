#!/bin/bash

# create base ROM image
bash make_padding.pl > rom

# 64k image
dd if=../../out/monitor.bin of=rom bs=1k seek=0 conv=sparse,nocreat,notrunc

# 64k image
dd if=../../out/basic.bin of=rom bs=1k seek=64 conv=sparse,nocreat,notrunc

# 64k image
dd if=../../out/stub.bin of=rom bs=1k seek=128 conv=sparse,nocreat,notrunc

# 16k image
dd if=../../out/stub.bin of=rom bs=1k seek=192 conv=sparse,nocreat,notrunc

# split the ROM image ready for burning
perl ../../bin/split --file=rom
