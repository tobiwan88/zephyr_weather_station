#!/bin/bash
set -e

# Initialize environment
echo "Starting Zephyr Weather Station Container..."
echo "Working directory: $(pwd)"
echo "Current user: $(whoami)"

# Activate Python virtual environment where West is installed
if [ -f "/home/zephyr/.venv/bin/activate" ]; then
    source /home/zephyr/.venv/bin/activate
    echo "✅ Python virtual environment activated"
else
    echo "⚠️  Warning: Python virtual environment not found at /home/zephyr/.venv/bin/activate"
fi

# Verify West is available
if command -v west >/dev/null 2>&1; then
    echo "✅ West command available: $(west --version)"
else
    echo "❌ West command not found in PATH"
    echo "Current PATH: $PATH"
fi

# Start MCP server in background
echo "🚀 Starting West MCP Server..."
if [ -f "/workspace/zephyr_weather_station/west_mcp_server.py" ]; then
    python3 /workspace/zephyr_weather_station/west_mcp_server.py > /tmp/mcp_server.log 2>&1 &
    MCP_PID=$!
    echo "✅ MCP Server started with PID: $MCP_PID"
    echo "📋 MCP Server logs: tail -f /tmp/mcp_server.log"

    # Create a cleanup function for graceful shutdown
    cleanup() {
        echo "🛑 Shutting down MCP Server (PID: $MCP_PID)..."
        kill $MCP_PID 2>/dev/null || true
        wait $MCP_PID 2>/dev/null || true
        echo "✅ MCP Server stopped"
    }
    trap cleanup EXIT SIGTERM SIGINT
else
    echo "⚠️  Warning: MCP Server script not found at /workspace/zephyr_weather_station/west_mcp_server.py"
fi

# Set up Cline if OPENROUTER_API_KEY is provided
if [ -n "$OPENROUTER_API_KEY" ]; then
    echo "Setting up Cline AI assistant..."
    /usr/local/bin/setup_cline.sh "${CLINE_MODEL:-devstral}"
else
    echo "No OPENROUTER_API_KEY provided - Cline setup skipped"
    echo "You can run 'setup_cline.sh' manually later if needed"
fi

# Verify Zephyr is available in the expected location
if [ -d "/workspace/zephyr" ]; then
    echo "✅ Zephyr found at /workspace/zephyr/"
else
    echo "⚠️  Zephyr not found at /workspace/zephyr/ - you may need to initialize it"
fi

# Change to the project directory
cd /workspace/zephyr_weather_station

echo "Container ready!"
echo ""
echo "Available commands:"
echo "  west init -l .     # Initialize Zephyr workspace (if not already done)"
echo "  west update        # Update dependencies"
echo "  cd app && west build -b native_sim  # Build for native simulator"
echo "  west build -t run  # Run the application"
echo ""
echo "Current directory: $(pwd)"

# If no command provided, start interactive bash
if [ $# -eq 0 ]; then
    echo "🔧 Toolchains: ${TOOLCHAINS:-arm-zephyr-eabi}"
    echo "💡 Usage: west init -m https://github.com/zephyrproject-rtos/zephyr --mr <version> <project>"
    exec /bin/bash
else
    # Execute the provided command
    exec "$@"
fi