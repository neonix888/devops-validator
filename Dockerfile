# Multi-stage Dockerfile for devops-validator
# Demonstrates containerization best practices

# Stage 1: Builder
FROM ubuntu:24.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /build

# Copy source code
COPY . .

# Build the project
RUN cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF && \
    cmake --build build --config Release

# Stage 2: Runtime
FROM ubuntu:24.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Copy binary from builder
COPY --from=builder /build/build/devops-validator /usr/local/bin/

# Create non-root user (let system assign UID)
RUN useradd -m devops && \
    mkdir -p /workspace && \
    chown -R devops:devops /workspace

# Switch to non-root user
USER devops
WORKDIR /workspace

# Set entrypoint
ENTRYPOINT ["devops-validator"]
CMD ["help"]

# Metadata
LABEL maintainer="neonix888"
LABEL description="DevOps Validator - Multi-platform validation CLI tool"
LABEL version="1.0.0"
