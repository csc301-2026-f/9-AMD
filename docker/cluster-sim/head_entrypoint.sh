#!/usr/bin/env bash
set -euo pipefail

echo "══════════════════════════════════════════════════════"
echo "  RAY HEAD NODE — starting"
echo "══════════════════════════════════════════════════════"

# 1. Start mock-UE gRPC server (used by the training driver for space
#    discovery, and also available if Ray places an env_runner here).
python /opt/sim/mock_ue_server.py --port "${SCHOLA_GRPC_PORT:-50051}" &
UE_PID=$!
sleep 1
echo "[head] Mock-UE server started (pid $UE_PID)"

# 2. Start Ray head daemon.
ray start --head \
    --port=6379 \
    --dashboard-host=0.0.0.0 \
    --num-cpus="${HEAD_CPUS:-1}"

echo "[head] Ray head is up."

# 3. Wait for workers to register.
WAIT_SECS="${WORKER_WAIT:-30}"
echo "[head] Waiting ${WAIT_SECS}s for workers to join the cluster…"
sleep "$WAIT_SECS"

# 4. Run the training driver.
echo "[head] Launching training driver…"
python /opt/sim/train_driver.py
EXIT_CODE=$?

# 5. Cleanup.
echo "[head] Training exited with code $EXIT_CODE — cleaning up."
ray stop || true
kill "$UE_PID" 2>/dev/null || true

exit "$EXIT_CODE"
