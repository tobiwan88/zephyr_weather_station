# zephyr_weather_station

A test to generate a zephyr weather station firmware just using AI

## Project Structure

This is a Zephyr T1 project workspace managed by west. The application code is in the `app/` directory.

## Build Instructions

### Prerequisites

- west tool installed
- Zephyr SDK or Docker environment

### Initialize the workspace

```bash
west init -l .
west update
```

### Build for native_sim (64-bit)

**Important**: Build from the workspace root, not from the app directory.

```bash
# From workspace root (/workspace in container)
west build zephyr_weather_station/app -b native_sim/native/64 --pristine
```

### Run on native_sim

```bash
west build -t run
```

**Note**: Use `native_sim/native/64` as the board target for proper 64-bit native simulation.

## Docker Setup

### Building the Docker Image

**Prerequisites**: You need to have the local `zephyr-docker:arm` base image available. This is used instead of the remote version due to compatibility issues.

```bash
docker build -t zephyr_weather_station .
```

### Running the Container

**Important**: Always mount the parent directory (`workspace_home`) as `/workspace` to ensure:
- Zephyr RTOS is accessible at `/workspace/zephyr/`
- The MCP server can be found at `/workspace/zephyr_weather_station/`
- West commands work correctly from the workspace root

### MCP (Model Context Protocol) Server

The project includes an MCP server (`west_mcp_server.py`) for AI integration with the west build system. This enables:
- AI-assisted development using Cline or similar tools
- Automated build and configuration management
- Integration with VS Code and other editors supporting MCP

**Setup for Cline**:
1. Ensure the MCP server is accessible in the container
2. Configure Cline to use the west MCP server
3. Use `native_sim/native/64` as the target board for development
- All dependencies and modules are properly accessible

#### With OpenRouter API Key (for Cline AI assistant)

To use the Cline AI assistant with OpenRouter, provide your API key as an environment variable:

```bash
# Using default model (Mistral Devstral 2512)
# From the workspace_home directory (parent of zephyr_weather_station)
docker run -it -v $(pwd):/workspace -e OPENROUTER_API_KEY=your_api_key_here zephyr_weather_station

# Using Qwen3 235B model
# From the workspace_home directory (parent of zephyr_weather_station)
docker run -it -v $(pwd):/workspace \
  -e OPENROUTER_API_KEY=your_api_key_here \
  -e CLINE_MODEL=qwen \
  zephyr_weather_station
```

**Important**: Never commit your API key to the repository. The key is only passed at runtime via the environment variable.

#### Without API Key (manual configuration needed)

```bash
# From the workspace_home directory (parent of zephyr_weather_station)
docker run -it -v $(pwd):/workspace zephyr_weather_station
```

Cline setup will be skipped, but you can run `setup_cline.sh` manually inside the container if needed later.

### Inside the Container

Once inside the container, initialize the Zephyr workspace and build:

```bash
west init -l .
west update
cd app
west build -b native_sim
west build -t run
```

## Cline AI Assistant Configuration

The project is pre-configured to use Cline AI assistant with OpenRouter. Two model options are available:

- **Default**: Mistral Devstral 2512 (free tier) - optimized for development tasks
- **Alternative**: Qwen QwQ 32B (free tier) - advanced reasoning model

### Automatic Setup

When you run the container with an `OPENROUTER_API_KEY`, Cline will be automatically configured using the official `cline auth` command. This ensures proper authentication setup and model configuration.

### Configuration Files

- `cline/cline_mcp_settings.json` - MCP server configuration (optional)
- `cline/setup_cline.sh` - Setup script that runs on container start

The authentication is handled by the Cline CLI itself, which is much more reliable than manually creating configuration files.

### Getting an OpenRouter API Key

1. Sign up at [openrouter.ai](https://openrouter.ai/)
2. Generate an API key from your account settings
3. Pass it to the Docker container via the `OPENROUTER_API_KEY` environment variable

### Selecting a Model

You can select which model to use via the `CLINE_MODEL` environment variable:

```bash
# Use default Devstral model
# From workspace_home directory
docker run -it -v $(pwd):/workspace -e OPENROUTER_API_KEY=your_key zephyr_weather_station

# Use Qwen3 model
# From workspace_home directory
docker run -it -v $(pwd):/workspace \
  -e OPENROUTER_API_KEY=your_key \
  -e CLINE_MODEL=qwen \
  zephyr_weather_station
```

### Using a Different Model

To add or modify model configurations, edit the template files:

**Default (Devstral)**:
```json
{
  "openRouterModelId": "mistralai/devstral-2512:free"
}
```

**Qwen3 (Alternative)**:
```json
{
  "openRouterModelId": "qwen/qwen3-235b-a22b:free"
}
```

Available models can be found at [openrouter.ai/models](https://openrouter.ai/models).

## Container Features

### Fixed Issues

- ✅ **West Commands Available**: The container now properly activates the Python virtual environment where West is installed
- ✅ **MCP Server Auto-Start**: The West MCP Server automatically starts in the background for AI assistant integration
- ✅ **Proper Environment Setup**: All Zephyr environment variables and toolchains are properly configured

### Testing Container Functionality

To verify that the container is working correctly, you can run the built-in test script:

```bash
# Inside the container
test_container.sh
```

This will check:
- West command availability
- Python environment configuration
- MCP Server functionality
- Environment variables setup

### MCP Server

The container includes a West MCP (Model Context Protocol) Server that provides:
- **Secure West Commands**: Allows `build`, `update`, `status`, `twister`
- **Blocked Dangerous Commands**: Prevents `flash`, `debug`, `debugserver`, `attach`
- **Git Integration**: Basic git commands for version control
- **JSON-RPC Interface**: Compatible with AI assistants and IDEs

The MCP server logs are available at `/tmp/mcp_server.log` inside the container.

### Environment Variables

The container sets up the following environment variables automatically:
- `ZEPHYR_TOOLCHAIN_VARIANT=zephyr`
- `ZEPHYR_SDK_INSTALL_DIR=/home/zephyr/zephyr-sdk`
- `PATH` includes `/home/zephyr/.venv/bin` for Python tools

### Volume Mounting Strategy

**Critical**: Always mount the parent directory (`workspace_home`) as `/workspace`:

```bash
# Correct mounting (from workspace_home directory)
cd workspace_home
docker run -it -v $(pwd):/workspace zephyr_weather_station
```

This ensures:
- ✅ **Zephyr RTOS**: Available at `/workspace/zephyr/`
- ✅ **Project Code**: Available at `/workspace/zephyr_weather_station/`
- ✅ **MCP Server**: Found at `/workspace/zephyr_weather_station/west_mcp_server.py`
- ✅ **Modules**: All dependencies accessible at `/workspace/modules/`
- ✅ **Tools**: Build tools available at `/workspace/tools/`

**Why this structure?**
- Zephyr's `west` tool expects to find dependencies at the workspace root level
- The MCP server can locate and interact with all project components
- AI assistants can navigate the complete project structure
- Builds can access all required modules and toolchains
