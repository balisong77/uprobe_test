```
objdump -T utest |grep utest_add
clang -g -O2 -target bpf -D__TARGET_ARCH_x86 -I/usr/include/x86_64-linux-gnu -I. -c uprobe.bpf.c -o uprobe.bpf.o
bpftool gen skeleton uprobe.bpf.o > uprobe.skel.h
```