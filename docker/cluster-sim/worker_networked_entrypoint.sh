#!/usr/bin/env bash
set -euo pipefail

HEAD_ADDR="${RAY_HEAD_ADDRESS:-ray-head:6379}"

echo "══════════════════════════════════════════════════════"
echo "  RAY WORKER (networked) — joining cluster at ${HEAD_ADDR}"
echo "══════════════════════════════════════════════════════"

# No local mock-UE server — it runs in a separate container.
# The ScholaEnvRunner resolves the UE address from the URL template
# in env_config (e.g. "ue-{worker_index}") at make_env time.
MAX_RETRIES=30
JOINED_CLUSTER=false
for i in $(seq 1 "$MAX_RETRIES"); do
    if ray start --address="$HEAD_ADDR" --num-cpus="${WORKER_CPUS:-1}" --resources='{"ue_worker": 1}'; then
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

echo "[worker] Idle — env_runner will connect to its UE instance over the network."
tail -f /dev/null
