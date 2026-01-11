# Cline Rules for Zephyr Weather Station Project

## Project Context
This is a Zephyr RTOS weather station application using native_sim/native/64 for development.

## Build Commands
- **Virtual Environment**: First activate with `source /home/zephyr/.venv/bin/activate`
- **Environment Setup**: Then source `source zephyr/zephyr-env.sh`
- **Build Location**: Always execute from workspace root (`/workspace` in container)
- **Build Command**: `west build zephyr_weather_station/app -b native_sim/native/64 --pristine`
- **Target Board**: Use `native_sim/native/64` exclusively for development
- **Clean Builds**: Always use `--pristine` flag for reliable builds
- **Never build from**: app subdirectory - this causes path issues

## File Operations
- **Read First**: Always use `read_file` to check current contents before editing
- **Context**: Include 3-5 lines before/after when using `replace_in_file`
- **Verify Changes**: Check file contents after edits to ensure success

## Zephyr Development Guidelines
- **Coding Standards**: Follow Zephyr conventions (snake_case, SPDX headers)
- **Kconfig**: Define symbols in `app/Kconfig` before using in `app/prj.conf`
- **Dependencies**: Check `west.yml` and `app/prj.conf` for required modules
- **APIs**: Use Zephyr APIs (zbus, Device Tree, logging subsystem)
- **Error Handling**: Use proper Zephyr error codes and patterns

## Container Workflow
- **Container Path**: Work from `/workspace` inside Docker container
- **Host Path**: Project located at `/Users/tobias.meyer/Documents/workspace_home/`
- **Mount Point**: Container mounts workspace_home as `/workspace`
- **Virtual Environment**: Always activate Python venv first: `source /home/zephyr/.venv/bin/activate`
- **Environment Setup**: Source Zephyr environment: `source zephyr/zephyr-env.sh`
- **Commands**: Execute via terminal commands, not MCP server

## Debugging and Troubleshooting
- **Kconfig Errors**: Check symbol definitions in `app/Kconfig`
- **Build Failures**: Use `--pristine` flag and verify workspace location
- **Module Issues**: Check `west.yml` for required dependencies
- **Path Problems**: Ensure building from workspace root, not app directory

## Rule Management
- **Rule Updates**: Cline should proactively suggest improvements to these rules based on:
  - Encountered issues and their solutions
  - Repeated patterns in development workflow
  - New discoveries about Zephyr development best practices
  - Container and build system optimizations
- **Rule Review**: Periodically review and update rules for accuracy and completeness
- **Experience Integration**: Incorporate lessons learned from debugging sessions into rules