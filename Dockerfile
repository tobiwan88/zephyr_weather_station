FROM ghcr.io/tobiwan88/zephyr_docker:arm

# Install Node.js and npm
RUN apt-get update && \
    apt-get install -y curl && \
    curl -fsSL https://deb.nodesource.com/setup_20.x | bash - && \
    apt-get install -y nodejs && \
    rm -rf /var/lib/apt/lists/*

# Install cline CLI globally
RUN npm install -g @cline/cli

# Install pyserial
RUN pip3 install pyserial

# Set working directory
WORKDIR /workspace

# Set environment variable for cline
ENV CLINE_DIR=/cline

# Create cline directory
RUN mkdir -p /cline
