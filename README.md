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

To use the Cline AI assistant with OpenRouter (Qwen3 model), provide your API key as an environment variable:

```bash
docker run -it -v $(pwd):/workspace -e OPENROUTER_API_KEY=your_api_key_here zephyr_weather_station
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

The project is pre-configured to use Cline AI assistant with OpenRouter and the Qwen 2.5 72B model.

### Configuration Files

- `cline/cline_api_config.template.json` - Template configuration (committed to repo, no secrets)
- `cline/cline_api_config.json` - Active configuration (generated at runtime, never committed)
- `cline/cline_mcp_settings.json` - MCP server configuration
- `cline/setup_cline.sh` - Setup script that runs on container start

### Getting an OpenRouter API Key

1. Sign up at [openrouter.ai](https://openrouter.ai/)
2. Generate an API key from your account settings
3. Pass it to the Docker container via the `OPENROUTER_API_KEY` environment variable

### Using a Different Model

Edit `cline/cline_api_config.template.json` to change the model:

```json
{
  "openRouterModelId": "qwen/qwen-2.5-72b-instruct"
}
```

Available models can be found at [openrouter.ai/models](https://openrouter.ai/models).
