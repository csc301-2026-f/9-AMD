#!/usr/bin/env bash
set -euo pipefail

HEAD_ADDR="${RAY_HEAD_ADDRESS:-ray-head:6379}"

echo "══════════════════════════════════════════════════════"
echo "  RAY LEARNER — starting  (head=${HEAD_ADDR})"
echo "══════════════════════════════════════════════════════"

MAX_RETRIES=30
for i in $(seq 1 "$MAX_RETRIES"); do
    if ray start --address="$HEAD_ADDR" --num-cpus="${LEARNER_CPUS:-2}"; then
        echo "[learner] Joined Ray cluster on attempt $i."
        break
    fi
    echo "[learner] Head not ready — retrying ($i/$MAX_RETRIES)…"
    sleep 3
done

echo "[learner] Idle — waiting for learner tasks from PPO algorithm."
tail -f /dev/null
