#!/bin/zsh

TIMEFMT='%mE'
BINARY=${1:-./dcf}
RUNS=${2:-10}
TOTAL=0
FLAGS=("${@:3}")

# Default flag if none provided
if [[ ${#FLAGS} -eq 0 ]]; then
    FLAGS=(-w12)
fi

echo "Binary:     $BINARY"
echo "Flags:      ${FLAGS[@]}"
echo "Runs:       $RUNS"
echo ""

for i in {1..$RUNS}; do
    RAW=$( { time (echo "1000000 1200" | taskset -c 0-11 $BINARY "${FLAGS[@]}" -z) } 2>&1 )
    MS=${RAW%ms}
    TOTAL=$((TOTAL + MS))
done

AVG=$(echo "scale=2; $TOTAL / $RUNS" | bc)
echo ""
echo "Average: ${AVG}ms over $RUNS runs ($BINARY ${FLAGS[@]})"
