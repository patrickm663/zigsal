#! /bin/bash

# First, get .c out
sisalc -keep -no-bounds -aggvector -vector -p=12 -o $1 $1.sis -lpthread

# Only keep .c
rm ./$1 $1.if1 $1.mem $1.mono $1.o $1.opt $1.part $1.up

# Then compile with Zig and several compiler flags
zig cc -O3 -march=native -ffast-math -flto=thin -fno-math-errno \
  -mllvm -enable-loopinterchange -mllvm -enable-loop-distribute \
  -funroll-loops -mprefer-vector-width=256 -mllvm -force-vector-interleave=4 \
  -mllvm -prefetch-distance=8 -mllvm --loop-prefetch-writes \
  -o $1 /usr/local/lib/sisal/srt0.o $1.c \
  -I/usr/local/include/sisal \
  -L/usr/local/lib/sisal \
  -Wl,--whole-archive -lsisal
