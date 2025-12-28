#!/bin/bash
# Super Paranoid Stress Test 2.0 for S3/Vintage sh (AMD64)
# This specific S3 build is vintage V7-style (1978).

SH="./sh"
RECURSION_LIMIT=500
CHURN_COUNT=50000
JOB_COUNT=100

echo "=== S3/Vintage Super Paranoid Stress Suite 2.0 ==="

# 1. Process Recursion
echo "--- Test 1: Process Recursion ($RECURSION_LIMIT levels) ---"
cat > recurse_s3.sh << 'EOF'
x=$1
if test "$x" = ""; then x=1; fi
mod=`expr $x % 100`
if test "$mod" -eq 0; then
    echo "Depth $x..."
fi
if test "$x" != "500"; then
    ./sh recurse_s3.sh `expr $x + 1`
fi
EOF
chmod +x recurse_s3.sh
./sh recurse_s3.sh 1
rm -f recurse_s3.sh

# 2. Lightning Churn (50k variables)
echo "--- Test 2: Lightning Churn ($CHURN_COUNT variables) ---"
CHURN_SCRIPT="/tmp/churn_stress_s3.sh"
echo "echo 'Generating churn script...'"
python3 -c "for i in range($CHURN_COUNT): print(f'V_{i}=val_{i}')" > "$CHURN_SCRIPT"
echo "echo 'Churning memory...'" >> "$CHURN_SCRIPT"

$SH "$CHURN_SCRIPT" > /dev/null

if test $? -eq 0; then
    echo "PASS: Lightning Churn"
else
    echo "FAIL: Lightning Churn"
    rm -f "$CHURN_SCRIPT"
    exit 1
fi
rm -f "$CHURN_SCRIPT"

# 3. Command Substitution
echo "--- Test 3: Command Substitution (1 level) ---"
RES=$($SH -c "echo \`echo bottom\`")
if test "$RES" = "bottom"; then
    echo "PASS: CmdSubst"
else
    echo "FAIL: CmdSubst (received: $RES)"
    exit 1
fi

# 4. Job Storm
echo "--- Test 4: Job Storm ($JOB_COUNT background procs) ---"
$SH << EOF
i=0
while test \$i -lt $JOB_COUNT; do
    (sleep 1) &
    i=\`expr \$i + 1\`
done
echo "Spawned $JOB_COUNT jobs. Waiting..."
wait
echo "Storm cleared."
EOF

if test $? -eq 0; then
    echo "PASS: Job Storm"
else
    echo "FAIL: Job Storm"
    exit 1
fi

echo "=== ALL S3 PARANOID TESTS PASSED ==="
