# zigsal
Benchmarking `sisalc`, `zig cc`, and `gcc` to compile Sisal programmes.

## Sisal?
Sisal is a long-forgotten language from the mid-80s, designed with supercomputers in mind. It is a functional language -- more specifically a dataflow language -- that compiles down to a dataflow graph (`.if1`) before eventually compiling to C.

Language-wise, it reads well and does things like parallelisation very well. Unfortunately, the project died in the 90s/early 00s. As such, there's virtually no modern-day discussions around the language. This is an attempt to kick some life into the project -- because I think it's pretty cool.

I use Sisal 14.1.0. See my [other repo](https://github.com/patrickm663/hello-sisal) for an installation guide (Linux) and some small programmes. More to follow. Alternatively, pick it up directly from Source Forge and hack away: https://sourceforge.net/projects/sisal/.

Big thanks to Pat Miller for keeping this alive and putting `sisalc` up on Source Forge :)

My C isn't too great, but I am picking up things as I go and learning things about compilers along the way. Ultimately, I want to write a new compiler for the IF1 dataflow graphs and get it into MLIR/XLA, bypassing C. From what I've read, it is similar to XLA's HLO graph. This could (theoretically) let us write Sisal that targets GPU and TPUs (!).

## Compiling the Normal Way
Using `sisalc`, one can compile `foo.sis` and output an executable `./foo` by running
```bash
sisalc -o foo foo.sis # May need to add -l<C lib> if calling C libs e.g. C's math library is -lm
```
Additional compiler flags let us turn off bound-checking and apply some optimisations:
```bash
sisalc -no-bounds -aggvector -vector -p=<No. Threads> -o foo foo.sis
```
To run it, we pipe in any input we want (space seperated):
```bash
echo "<my args>" | ./foo
```
Or, using `foo.in` rather than `echo`:
```bash
./foo < foo.in > 2>&1
```
Optionally, we can use the flag `-wX` to set the number of threads:
```bash
echo "<my args>" | ./foo -w12 # Running on 12 threads
```
We can take it one further and put `taskset` in front of the executable call to get a little more performance out of it:
```bash
echo "<my args>" | taskset -c 0-11 ./foo -w12
```
Now, to extract the intermediate `foo.c` file, we can run:
```bash
sisalc -C -forC foo.sis
```
Interested in more intermediate files? `-keep` can be used to compile the programme and keep all the files:
```bash
sisalc -keep -o foo foo.sis
```
The above works in combination with optimisation flags. The resultant C code can be checked to see if e.g. `-no-bounds` did its job.
## Compiling foo.c using Zig
This works pretty well, but what if we didn't want to use GCC?

We can use Zig as a drop-in C compiler for `foo.c` as follows:
```bash
sisalc -keep -o foo foo.sis # For some reason -forC produces errors when calling zig cc
zig cc -o foo /usr/local/lib/sisal/srt0.o foo.c \
    -I/usr/local/include/sisal \
    -L/usr/local/lib/sisal \
    -Wl,--whole-archive -lsisal -Wl,--no-whole-archive
# Clean-up
rm foo.mem foo.mono foo.o foo.opt foo.part foo.up
```
You might find the Zig compiled binary is actually _slower_ that `sisalc`'s since we don't use any optimisers. We can pass flags to `zig cc` as we would any olf `foo.c`. Below are some I found to really crank up the performance just to see what : 
```bash
zig cc -O3 -march=native -ffast-math -flto=thin -fno-math-errno \
  -mllvm -enable-loopinterchange -mllvm -enable-loop-distribute \
  -funroll-loops -mprefer-vector-width=256 -mllvm -force-vector-interleave=4 \
  -mllvm -prefetch-distance=8 -mllvm --loop-prefetch-writes \
  -o foo /usr/local/lib/sisal/srt0.o foo.c \
  -I/usr/local/include/sisal \
  -L/usr/local/lib/sisal \
  -Wl,--whole-archive -lsisal
```
The above isn't neccessary in all cases, like `-ffast-math` could lead to imprecision at the decimal point if floating point errors accumulate. Below is a summary of each flag (Claude helped here):
- `-O3` : Maximum compiler optimisation; enables auto-vectorisation, aggressive inlining, and loop transformations
- `-march=native` : Emit instructions for the exact CPU running the build, enabling AVX2/AVX-512 and other extensions not available in generic x86-64 code
- `-ffast-math` : Relaxes strict IEEE 754 floating-point rules, allowing reordering and approximation of float operations for speed
- `-flto=thin` : Enables ThinLTO (Link Time Optimisation); allows the compiler to optimise across translation unit boundaries at link time, with lower memory cost than full LTO
- `-fno-math-errno` : Skips setting `errno` after math functions like `sqrt`; removes a hidden branch and memory write after every such call
- `-mllvm -enable-loopinterchange` : Allows LLVM to swap the order of nested loops to improve cache locality (e.g. making the inner loop stride sequentially through memory)
- `-mllvm -enable-loop-distribute` : Splits loops with multiple independent bodies into separate loops, enabling each part to be vectorised independently
- `-funroll-loops` : Unrolls loop bodies to reduce branch overhead and expose more instruction-level parallelism to the CPU
- `-mprefer-vector-width=256` : Instructs LLVM to prefer 256-bit (AVX2) vector operations over 128-bit or 512-bit, balancing throughput against frequency throttling on most desktop CPUs
- `-mllvm -force-vector-interleave=4` : Interleaves 4 vector iterations in the loop body, hiding memory latency by keeping the CPU's out-of-order execution units busy
- `-mllvm -prefetch-distance=8` : Inserts software prefetch instructions 8 iterations ahead of the current loop position, reducing cache miss stalls
- `-mllvm --loop-prefetch-writes` : Extends prefetching to write targets as well as reads, useful when the loop both reads and writes large arrays

## Convenience Wrappers
Included in this repo is `zig-compile-sisal.sh` which just makes it easier rather than typing out the commands.

The shell script takes `foo` as an input and returns `./foo`: 
```bash
bash zig-compile-sisal.sh foo
```
This uses the LLVM flags. They can be removed if not needed.

## What about GCC?
So, the `gcc` implementation looks as follows and is very similar to `zig cc` -- an important difference being the linkers after `-lsisal`:
```bash
gcc -o dcf /usr/local/lib/sisal/srt0.o dcf.c \
  -I/usr/local/include/sisal \
  -L/usr/local/lib/sisal \
  -Wl,--whole-archive -lsisal -Wl,--no-whole-archive \
  -lm
```
Optimisation flags also behave similarly -- minus the LLVM flags:
```bash
gcc -O3 -march=native -ffast-math -flto=auto -fno-math-errno \
  -floop-interchange -floop-strip-mine \
  -funroll-loops -mprefer-vector-width=256 \
  -fprefetch-loop-arrays \
  -o dcf /usr/local/lib/sisal/srt0.o dcf.c \
  -I/usr/local/include/sisal \
  -L/usr/local/lib/sisal \
  -Wl,--whole-archive -lsisal -Wl,--no-whole-archive \
  -lm
```
As the benchmark shows below, `zig cc` seems to outperform `gcc` when we turn the optimisers on (!?), likely owing to LLVM and some better optimisation under the hood. 
## Benchmark 
We have a Sisal programme `dcf.sis` that calculates $V = CF \times D$, where $CF$ is a matrix of $n\times m$ 'cashflows' and $D$ is a vector of $m$ 'discount rates'. Since we don't have built-in matrix algebra, we write this by hand using loops. The programme accepts a value for $n$ and $m$. The values in the matrix are populated _during_ execution.

Note that this is a toy problem to show the differences in run-time under different compilations. This may change depending on problem, but I have found a 'nerd-sniped' `zig cc` call to produce the fastest executables for Sisal programmes. YMMV.

## Specs
- CPU: 11th Gen Intel i5-11400H (12) @ 4.500GHz
- Memory: 16GB RAM
- OS: Pop!\_OS 24.04 LTS x86\_64
- Kernel: 6.18.7-76061807-generic
- GCC: 13.3.0
- Zig: 0.12.0-dev.2644+42fcca49c
- Sisal: 14.1.0

### TL;DR
| # | Compiler | Compile Flags | Execution | Wall Time | CPU% |
|---|----------|--------------|-----------|-----------|------|
| 1 | `sisalc` | *(default)* | single-threaded | 1.886s | 99% |
| 2 | `sisalc` | *(default)* | `-w12` + `taskset` | 0.523s | 717% |
| 3 | `sisalc` | `-no-bounds -aggvector -vector -p=12` | `-w12` + `taskset` | 0.418s | 628% |
| 4 | `zig cc` | *(none)* | single-threaded | 24.322s | 99% |
| 5 | `zig cc` | *(none)* | `-w12` + `taskset` | 4.609s | 1108% |
| 6 | `zig cc` | `-O3` | single-threaded | 1.437s | 99% |
| 7 | `zig cc` | `-O3` | `-w12` + `taskset` | 0.394s | 584% |
| 8 | `zig cc` | `-O3 -march=native -ffast-math -flto=thin` + LLVM flags | single-threaded | 0.493s | 99% |
| 9 | `zig cc` | `-O3 -march=native -ffast-math -flto=thin` + LLVM flags | `-w12` + `taskset` | 0.246s | 266% |
| 10 | `gcc` | *(none)* | single-threaded | 2.484s | 99% |
| 11 | `gcc` | *(none)* | `-w12` + `taskset` | 0.661s | 816% |
| 12 | `gcc` | `-O3` | single-threaded | 1.542s | 99% |
| 13 | `gcc` | `-O3` | `-w12` + `taskset` | 0.478s | 663% |
| 14 | `gcc` | `-O3 -march=native -ffast-math -flto=auto` + loop flags | single-threaded | 1.433s | 99% |
| 15 | `gcc` | `-O3 -march=native -ffast-math -flto=auto` + loop flags | `-w12` + `taskset` | 0.397s | 598% |

Overall, optimised `sisalc` compilation with multi-threading (a freebie by design) can get you pretty far. Compared to plain `zig cc` and `gcc`, it seems to apply some of its own optimisations. Zig with `-O3` takes it up a notch, but the whole set of flags make a difference in run-time, about 1.5x the fastest I could get with `sisalc`-only optimisation. `gcc` had a better 'unoptimised' run than `zig cc`, but worse than `sisalc`. 

These numbers also depend on some environmental factors so they really just give a general idea. Overall, ~250ms is pretty decent on fairly large matrix-vector multiplication using for-loops!

