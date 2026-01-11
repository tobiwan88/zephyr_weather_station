#!/bin/bash
# Test script to verify container functionality

echo "🧪 Testing container functionality..."

echo "1. Testing West availability:"
if command -v west >/dev/null 2>&1; then
    echo "✅ West found: $(west --version)"
else
    echo "❌ West not found"
    exit 1
fi

echo "2. Testing Python environment:"
python3 -c "import west; print('✅ West Python module available')" || echo "❌ West Python module not available"

echo "3. Testing MCP Server availability:"
if [ -f "/workspace/zephyr_weather_station/west_mcp_server.py" ]; then
    echo "✅ MCP Server script found"
    if [ -x "/workspace/zephyr_weather_station/west_mcp_server.py" ]; then
        echo "✅ MCP Server script is executable"
    else
        echo "⚠️  MCP Server script exists but not executable"
    fi
else
    echo "❌ MCP Server script not found"
fi

echo "4. Testing MCP Server functionality (quick test):"
echo '{"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {}}' | timeout 5s python3 /workspace/zephyr_weather_station/west_mcp_server.py | head -1 && echo "✅ MCP Server responds" || echo "⚠️  MCP Server test failed"

echo "5. Checking environment variables:"
echo "ZEPHYR_TOOLCHAIN_VARIANT: ${ZEPHYR_TOOLCHAIN_VARIANT:-not set}"
echo "ZEPHYR_SDK_INSTALL_DIR: ${ZEPHYR_SDK_INSTALL_DIR:-not set}"
echo "PATH contains venv: $(echo $PATH | grep -o '/home/zephyr/.venv/bin' || echo 'not found')"

echo "🏁 Container test completed"