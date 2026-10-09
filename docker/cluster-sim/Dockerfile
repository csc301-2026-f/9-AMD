FROM python:3.11-slim

# System packages needed to build grpcio wheels
RUN apt-get update && \
    apt-get install -y --no-install-recommends build-essential && \
    rm -rf /var/lib/apt/lists/*

# Install CPU-only PyTorch first (avoids downloading 5 GB of CUDA libraries).
RUN pip install --no-cache-dir torch --index-url https://download.pytorch.org/whl/cpu

# Install the Schola Python package (with RLlib extras) from the repo source.
# torch is already satisfied above so pip won't re-download it.
COPY Resources/python /opt/schola-src
RUN pip install --no-cache-dir "/opt/schola-src[rllib]"

# Copy the test mock-server infrastructure (proven in unit tests).
COPY Test/envs /opt/test_envs

# Copy the cluster-simulation scripts.
COPY docker/cluster-sim/*.py  /opt/sim/
COPY docker/cluster-sim/*.sh  /opt/sim/
RUN chmod +x /opt/sim/*.sh

ENV PYTHONPATH="/opt:${PYTHONPATH}"
WORKDIR /opt/sim
