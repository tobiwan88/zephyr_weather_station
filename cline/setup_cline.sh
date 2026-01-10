#!/bin/bash
# Cline configuration script
# This script sets up Cline with OpenRouter API key from environment variable
# Usage: setup_cline.sh [model]
#   model: Optional model selection (default: devstral, options: devstral, qwen)

set -e

CLINE_CONFIG_DIR="${CLINE_DIR:-/cline}"
CONFIG_FILE="$CLINE_CONFIG_DIR/cline_api_config.json"

# Determine the script directory for finding templates
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Determine which template to use based on argument or environment variable
MODEL_CHOICE="${1:-${CLINE_MODEL:-devstral}}"

case "$MODEL_CHOICE" in
    qwen)
        TEMPLATE_FILE="$SCRIPT_DIR/cline_api_config.template.qwen.json"
        echo "Using Qwen3 235B model configuration"
        ;;
    devstral|*)
        TEMPLATE_FILE="$SCRIPT_DIR/cline_api_config.template.json"
        echo "Using Mistral Devstral 2512 model configuration (default)"
        ;;
esac

MCP_TEMPLATE_FILE="$SCRIPT_DIR/cline_mcp_settings.json"

# Create CLINE_CONFIG_DIR if it doesn't exist
mkdir -p "$CLINE_CONFIG_DIR"

# Check if OPENROUTER_API_KEY is set
if [ -z "$OPENROUTER_API_KEY" ]; then
    echo "WARNING: OPENROUTER_API_KEY environment variable is not set"
    echo "Please set it when running the container:"
    echo "  docker run -e OPENROUTER_API_KEY=your_key_here ..."
    echo ""
    echo "Using template configuration without API key..."
    cp "$TEMPLATE_FILE" "$CONFIG_FILE"
else
    echo "Configuring Cline with OpenRouter API key..."
    # Create config from template and inject API key
    jq --arg key "$OPENROUTER_API_KEY" '.openRouterApiKey = $key' "$TEMPLATE_FILE" > "$CONFIG_FILE"
    echo "Cline configuration created at $CONFIG_FILE"
fi

# Also copy MCP settings
MCP_SETTINGS_FILE="$CLINE_CONFIG_DIR/cline_mcp_settings.json"
cp "$MCP_TEMPLATE_FILE" "$MCP_SETTINGS_FILE"
echo "MCP settings copied to $MCP_SETTINGS_FILE"

