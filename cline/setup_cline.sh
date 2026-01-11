#!/bin/bash
# Cline configuration script
# This script sets up Cline with OpenRouter API key from environment variable
# Usage: setup_cline.sh [model]
#   model: Optional model selection (default: devstral, options: devstral, qwen)

set -e

CLINE_CONFIG_DIR="${CLINE_DIR:-/cline}"

# Determine which model to use based on argument or environment variable
MODEL_CHOICE="${1:-${CLINE_MODEL:-devstral}}"

case "$MODEL_CHOICE" in
    qwen)
        MODEL_ID="qwen/qwq-32b-preview:free"
        echo "Using Qwen QwQ 32B model configuration (free)"
        ;;
    devstral|*)
        MODEL_ID="mistralai/devstral-2512:free"
        echo "Using Mistral Devstral 2512 model configuration (free)"
        ;;
esac

# Create CLINE_CONFIG_DIR if it doesn't exist
mkdir -p "$CLINE_CONFIG_DIR"

# Check if OPENROUTER_API_KEY is set
if [ -z "$OPENROUTER_API_KEY" ]; then
    echo "WARNING: OPENROUTER_API_KEY environment variable is not set"
    echo "Please set it when running the container:"
    echo "  docker run -e OPENROUTER_API_KEY=your_key_here ..."
    echo ""
    echo "Cline setup skipped. You can configure it manually later with:"
    echo "  cline auth openrouter"
else
    echo "Configuring Cline with OpenRouter API key..."

    # Test the OpenRouter API key first to ensure it's valid
    echo "Validating OpenRouter API key..."
    API_TEST=$(curl -s -H "Authorization: Bearer $OPENROUTER_API_KEY" \
                    -H "Content-Type: application/json" \
                    "https://openrouter.ai/api/v1/models" | head -c 100)

    if echo "$API_TEST" | grep -q '"data"'; then
        echo "✓ OpenRouter API key is valid"
    else
        echo "✗ OpenRouter API key validation failed"
        echo "Response: $API_TEST"
        echo "Please check your API key and try again"
        echo "You can test it manually with:"
        echo "  curl -H \"Authorization: Bearer YOUR_KEY\" https://openrouter.ai/api/v1/models"
        exit 1
    fi

    # Use cline auth to properly configure authentication
    # This is much better than manually creating JSON files
    # Use the provider flag with apikey for non-interactive setup
    echo "Setting up Cline authentication..."
    cline auth --provider openrouter --apikey "$OPENROUTER_API_KEY" --modelid "$MODEL_ID"

    if [ $? -eq 0 ]; then
        echo "Cline authentication configured successfully!"
        echo "Model: $MODEL_ID"
    else
        echo "Failed to configure Cline authentication. You may need to run 'cline auth' manually."
        echo "To configure manually, run: cline auth --provider openrouter --apikey YOUR_KEY --modelid $MODEL_ID"
    fi
fi

# Copy MCP settings if they exist
MCP_TEMPLATE_FILE="$CLINE_CONFIG_DIR/cline_mcp_settings.json"
if [ -f "$MCP_TEMPLATE_FILE" ]; then
    echo "MCP settings found at $MCP_TEMPLATE_FILE"
else
    echo "No MCP settings template found - skipping MCP configuration"
fi

