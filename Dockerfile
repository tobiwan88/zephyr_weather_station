FROM ghcr.io/tobiwan88/zephyr_docker:arm

# Install Node.js, npm, and jq for JSON processing
RUN apt-get update && \
    apt-get install -y curl jq && \
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

# Copy cline setup script
COPY cline/setup_cline.sh /usr/local/bin/setup_cline.sh
RUN chmod +x /usr/local/bin/setup_cline.sh

# Run setup script on container start
# Note: OPENROUTER_API_KEY should be provided as environment variable when running container
# Optional: CLINE_MODEL can be set to 'qwen' to use Qwen3 instead of default Devstral
CMD ["/bin/bash", "-c", "/usr/local/bin/setup_cline.sh \"${CLINE_MODEL:-devstral}\" && exec /bin/bash"]

