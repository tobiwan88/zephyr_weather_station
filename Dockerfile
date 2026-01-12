FROM zephyr-docker:arm

# Install Node.js, npm, and jq for JSON processing
USER root
RUN apt-get update && \
    apt-get install -y curl jq && \
    curl -fsSL https://deb.nodesource.com/setup_20.x | bash - && \
    apt-get install -y nodejs && \
    rm -rf /var/lib/apt/lists/*

# Install Cline CLI globally with the correct package name
RUN npm install -g cline

# Install pyserial and common Zephyr dependencies
RUN pip3 install pyserial jsonschema pykwalify pyyaml

# Create cline directory as root before switching users
RUN mkdir -p /cline && chown -R zephyr:zephyr /cline

# Copy scripts and make them executable as root
COPY cline/setup_cline.sh /usr/local/bin/setup_cline.sh
RUN chmod +x /usr/local/bin/setup_cline.sh

COPY docker-entrypoint.sh /usr/local/bin/docker-entrypoint.sh
COPY test_container.sh /usr/local/bin/test_container.sh
RUN chmod +x /usr/local/bin/docker-entrypoint.sh
RUN chmod +x /usr/local/bin/test_container.sh

# Ensure the zephyr user exists and has proper permissions
RUN usermod -aG sudo zephyr || true

# Switch back to zephyr user for security
USER zephyr

# Set working directory
WORKDIR /workspace

# Set environment variable for cline
ENV CLINE_DIR=/cline

# Copy all cline configuration files and scripts
COPY cline/ /cline/

# Set entrypoint
# Note: OPENROUTER_API_KEY should be provided as environment variable when running container
# Optional: CLINE_MODEL can be set to 'qwen' to use Qwen3 instead of default Devstral
ENTRYPOINT ["/usr/local/bin/docker-entrypoint.sh"]
CMD ["/bin/bash"]

