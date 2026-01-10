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

### Build for native_sim

```bash
cd app
west build -b native_sim
```

### Run on native_sim

```bash
west build -t run
```

## Docker Setup

### Building the Docker Image

```bash
docker build -t zephyr_weather_station .
```

### Running the Container

#### With OpenRouter API Key (for Cline AI assistant)

To use the Cline AI assistant with OpenRouter, provide your API key as an environment variable:

```bash
# Using default model (Mistral Devstral 2512)
docker run -it -v $(pwd):/workspace -e OPENROUTER_API_KEY=your_api_key_here zephyr_weather_station

# Using Qwen3 235B model
docker run -it -v $(pwd):/workspace \
  -e OPENROUTER_API_KEY=your_api_key_here \
  -e CLINE_MODEL=qwen \
  zephyr_weather_station
```

**Important**: Never commit your API key to the repository. The key is only passed at runtime via the environment variable.

#### Without API Key (manual configuration needed)

```bash
docker run -it -v $(pwd):/workspace zephyr_weather_station
```

You'll need to manually configure Cline after starting the container.

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
- **Alternative**: Qwen3 235B (free tier) - larger general-purpose model

### Configuration Files

- `cline/cline_api_config.template.json` - Default Devstral template (committed to repo, no secrets)
- `cline/cline_api_config.template.qwen.json` - Qwen3 template (backup option)
- `cline/cline_api_config.json` - Active configuration (generated at runtime, never committed)
- `cline/cline_mcp_settings.json` - MCP server configuration
- `cline/setup_cline.sh` - Setup script that runs on container start

### Getting an OpenRouter API Key

1. Sign up at [openrouter.ai](https://openrouter.ai/)
2. Generate an API key from your account settings
3. Pass it to the Docker container via the `OPENROUTER_API_KEY` environment variable

### Selecting a Model

You can select which model to use via the `CLINE_MODEL` environment variable:

```bash
# Use default Devstral model
docker run -it -v $(pwd):/workspace -e OPENROUTER_API_KEY=your_key zephyr_weather_station

# Use Qwen3 model
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
