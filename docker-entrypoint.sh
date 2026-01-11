#!/bin/bash
set -e

# Initialize environment
echo "Starting Zephyr Weather Station Container..."
echo "Working directory: $(pwd)"
echo "Current user: $(whoami)"

# Set up Cline if OPENROUTER_API_KEY is provided
if [ -n "$OPENROUTER_API_KEY" ]; then
    echo "Setting up Cline AI assistant..."
    /usr/local/bin/setup_cline.sh "${CLINE_MODEL:-devstral}"
else
    echo "No OPENROUTER_API_KEY provided - Cline setup skipped"
    echo "You can run 'setup_cline.sh' manually later if needed"
fi

echo "Container ready!"
echo ""
echo "Available commands:"
echo "  west init -l .     # Initialize Zephyr workspace"
echo "  west update        # Update dependencies"
echo "  cd app && west build -b native_sim  # Build for native simulator"
echo "  west build -t run  # Run the application"
echo ""

# Execute the passed command or default to bash
exec "$@"