#!/usr/bin/env bash
set -euo pipefail

echo "══════════════════════════════════════════════════════"
echo "  RAY HEAD (networked) — starting"
echo "══════════════════════════════════════════════════════"

# No local mock-UE server — space-discovery connects to ue-disc over the network.

# 1. Start Ray head daemon.
ray start --head \
    --port=6379 \
    --dashboard-host=0.0.0.0 \
    --num-cpus="${HEAD_CPUS:-1}"

echo "[head] Ray head is up."

# 2. Wait for workers to register.
WAIT_SECS="${WORKER_WAIT:-30}"
echo "[head] Waiting ${WAIT_SECS}s for workers to join the cluster…"
sleep "$WAIT_SECS"

# 3. Run the networked training driver.
echo "[head] Launching networked training driver…"
python /opt/sim/train_driver_networked.py
EXIT_CODE=$?

# 4. Cleanup.
echo "[head] Training exited with code $EXIT_CODE — cleaning up."
ray stop || true

exit "$EXIT_CODE"
