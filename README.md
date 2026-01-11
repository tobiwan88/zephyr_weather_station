# zephyr_weather_station

A test to generate a zephyr weather station firmware just using AI

## AI Assistant Integration

**IMPORTANT for Cline AI Assistant Users**:

### Rule Loading Issues with Mistral Devstral
If you're using Cline with `openrouter/mistralai/devstral-2512:free`, the model may not automatically read the `.cline_rules/` folder. To ensure proper rule application:

1. **Manual Rule Reference**: Explicitly mention rules in your prompts:
   ```
   "Please follow the rules in .cline_rules/basic.md for this Zephyr project"
   ```

2. **Key Rules Summary**: Always build from workspace root with these commands:
   ```bash
   # In container, always run these first:
   source /home/zephyr/.venv/bin/activate
   source zephyr/zephyr-env.sh

   # Then build:
   west build zephyr_weather_station/app -b native_sim/native/64 --pristine
   ```

3. **Alternative Models**: Consider using Claude or other models that better support rule folders

### Comprehensive Rules Available
This project includes comprehensive coding rules in the `.cline_rules/` folder. AI assistants should automatically read and apply these rules for:
- Proper build procedures
- Zephyr development best practices
- Container workflow guidelines
- File operation standards

The rules are designed to guide AI assistants in working effectively with Zephyr RTOS development.

## Project Structure

This is a Zephyr T1 project workspace managed by west. The application code is in the `app/` directory.

## Build Instructions

### Prerequisites

- west tool installed
- Zephyr SDK or Docker environment

### Initialize the workspace

**Note**: Initialization only needs to be done once when setting up the project. If the workspace is already initialized with all dependencies, you can skip directly to building.

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

Once inside the container, activate the Python virtual environment and set up the Zephyr environment:

```bash
# Required: Activate Python virtual environment (west tools are here)
source /home/zephyr/.venv/bin/activate

# Required: Source Zephyr environment variables
source zephyr/zephyr-env.sh

# If workspace is already initialized (most common case):
west build zephyr_weather_station/app -b native_sim/native/64 --pristine
west build -t run

# Only if starting fresh (workspace not initialized):
west init -l .
west update
west build zephyr_weather_station/app -b native_sim/native/64 --pristine
west build -t run
```

**Important**: The virtual environment activation is required for `west` commands to be available.

## Cline AI Assistant Configuration

The project is pre-configured to use Cline AI assistant with OpenRouter.

### Model Compatibility Notes

- **Mistral Devstral 2512** (`openrouter/mistralai/devstral-2512:free`): May not automatically read `.cline_rules/` folder. Manually reference rules in prompts.
- **Claude Models**: Better support for automatic rule loading
- **Qwen Models**: Good rule following capabilities

### Model Options Available

- **Default**: Mistral Devstral 2512 (free tier) - optimized for development tasks
- **Alternative**: Qwen QwQ 32B (free tier) - advanced reasoning model
- **Recommended**: Claude models (paid) - best rule adherence

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
