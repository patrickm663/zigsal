#!/bin/zsh

## sisalc
sisalc -o dcf_sisalc dcf.sis 1&>1
## sisalc opt
sisalc -no-bounds -aggvector -vector -p=12 -o dcf_sisalc_opt dcf.sis 1&>1
## gcc
sisalc -keep -o dcf_gcc dcf.sis 1&>1
gcc -o dcf_gcc /usr/local/lib/sisal/srt0.o dcf.c \
  -I/usr/local/include/sisal \
  -L/usr/local/lib/sisal \
  -Wl,--whole-archive -lsisal -Wl,--no-whole-archive \
  -lm 1&>1
## gcc -O3
sisalc -keep -o dcf_gcc_O3 dcf.sis 1&>1
gcc -O3 -o dcf_gcc_O3 /usr/local/lib/sisal/srt0.o dcf.c \
  -I/usr/local/include/sisal \
  -L/usr/local/lib/sisal \
  -Wl,--whole-archive -lsisal -Wl,--no-whole-archive \
  -lm 1&>1
## gcc opt
sisalc -keep -o dcf_gcc_opt dcf.sis 1&>1
gcc -O3 -march=native -ffast-math -flto=auto -fno-math-errno \
  -floop-interchange -floop-strip-mine \
  -funroll-loops -mprefer-vector-width=256 \
  -fprefetch-loop-arrays \
  -o dcf_gcc_opt /usr/local/lib/sisal/srt0.o dcf.c \
  -I/usr/local/include/sisal \
  -L/usr/local/lib/sisal \
  -Wl,--whole-archive -lsisal -Wl,--no-whole-archive \
  -lm 1&>1
## zig
sisalc -keep -o dcf_zig dcf.sis 1&>1
zig cc -o dcf_zig /usr/local/lib/sisal/srt0.o dcf.c -I/usr/local/include/sisal -L/usr/local/lib/sisal -Wl,--whole-archive -lsisal 1&>1
## zig -O3
sisalc -keep -o dcf_zig_O3 dcf.sis 1&>1
zig cc -O3 -o dcf_zig_O3 /usr/local/lib/sisal/srt0.o dcf.c -I/usr/local/include/sisal -L/usr/local/lib/sisal -Wl,--whole-archive -lsisal 1&>1
## zig opt
sisalc -keep -o dcf_zig_opt dcf.sis 1&>1
zig cc -O3 -march=native -ffast-math -flto=thin -fno-math-errno \
  -mllvm -enable-loopinterchange -mllvm -enable-loop-distribute \
  -funroll-loops -mprefer-vector-width=256 -mllvm -force-vector-interleave=4 \
  -mllvm -prefetch-distance=8 -mllvm --loop-prefetch-writes \
  -o dcf_zig_opt /usr/local/lib/sisal/srt0.o dcf.c \
  -I/usr/local/include/sisal \
  -L/usr/local/lib/sisal \
  -Wl,--whole-archive -lsisal 1&>1

## Clean up
rm 1 dcf.o dcf.mono dcf.opt dcf.part dcf.if1 dcf.up dcf.mem 1&>1
