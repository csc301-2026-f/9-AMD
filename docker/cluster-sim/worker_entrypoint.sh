#!/usr/bin/env bash
set -euo pipefail

HEAD_ADDR="${RAY_HEAD_ADDRESS:-ray-head:6379}"

echo "══════════════════════════════════════════════════════"
echo "  RAY WORKER — starting  (head=${HEAD_ADDR})"
echo "══════════════════════════════════════════════════════"

# 1. Start mock-UE gRPC server on the configured port.
python /opt/sim/mock_ue_server.py --port "${SCHOLA_GRPC_PORT:-50051}" &
UE_PID=$!
sleep 1
echo "[worker] Mock-UE server started (pid $UE_PID)"

# 2. Join the Ray cluster (retry until the head is reachable).
MAX_RETRIES=30
JOINED_CLUSTER=false
for i in $(seq 1 "$MAX_RETRIES"); do
    if ray start --address="$HEAD_ADDR" --num-cpus="${WORKER_CPUS:-1}" --resources='{"ue_sidecar": 1}'; then
        echo "[worker] Joined Ray cluster on attempt $i."
        JOINED_CLUSTER=true
        break
    fi
    echo "[worker] Head not ready — retrying ($i/$MAX_RETRIES)…"
    sleep 3
done
if [ "$JOINED_CLUSTER" != true ]; then
    echo "[worker] ERROR: Failed to join Ray cluster at ${HEAD_ADDR} after ${MAX_RETRIES} attempts." >&2
    exit 1
fi

echo "[worker] Idle — waiting for env_runner tasks from head."

# 3. Keep alive: wait for the mock-UE server.  If it exits unexpectedly
#    we surface the failure so docker-compose / K8s can detect it.
wait "$UE_PID"
UE_EXIT=$?
if [ "$UE_EXIT" -ne 0 ]; then
    echo "[worker] ERROR: Mock-UE server (pid $UE_PID) exited with code $UE_EXIT" >&2
    exit "$UE_EXIT"
fi
