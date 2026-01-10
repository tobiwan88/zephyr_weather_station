# Agent Rules for Zephyr Weather Station Development

## Overview

This document defines the rules and guidelines for AI agents working on the Zephyr Weather Station project.

## West Tool Usage

The project provides an MCP (Model Context Protocol) server that exposes a guarded `west` tool for safe command execution.

### Allowed West Commands

The following `west` commands are **ALLOWED** and can be executed freely:

- `west build` - Build the application
- `west update` - Update dependencies
- `west status` - Check workspace status  
- `west twister` - Run test automation

### Blocked West Commands

The following `west` commands are **BLOCKED** for security reasons:

- `west flash` - Flashing firmware to hardware (requires physical access)
- `west debug` - Starting debug sessions (requires hardware/emulator)
- `west debugserver` - Starting debug server
- `west attach` - Attaching to running processes

**Rationale**: These commands interact with physical hardware or require special privileges that should not be automated without explicit human oversight.

## Development Guidelines

### 1. Code Changes

- Always test builds using `west build -b native_sim` before committing
- Follow Zephyr coding standards and conventions
- Keep changes minimal and focused
- Document all configuration changes

### 2. Building

- Use `native_sim` board for local testing
- Always verify builds complete successfully
- Check for warnings and address them when possible

### 3. Testing

- Use `west twister` for automated testing when available
- Verify application runs correctly on `native_sim`
- Test any new features thoroughly

### 4. Documentation

- Update README.md when adding new features or changing build process
- Document any new configuration options in comments
- Keep build instructions up to date

### 5. Security

- Never commit secrets or credentials
- Follow secure coding practices
- Be cautious with external dependencies

## Workspace Structure

```
zephyr_weather_station/
├── app/                    # Application code (T1 workspace self path)
│   ├── CMakeLists.txt     # Build configuration
│   ├── prj.conf           # Project configuration
│   └── src/
│       └── main.c         # Main application
├── west.yml               # West manifest
├── west_mcp_server.py     # MCP server for guarded west access
├── cline/
│   └── cline_mcp_settings.json  # Cline MCP configuration
├── Dockerfile             # Docker development environment
└── README.md              # Project documentation
```

## MCP Server Usage

The `west_mcp_server.py` provides a JSON-RPC server that exposes the `west` tool through MCP. 

### Tool Interface

**Tool Name**: `west`

**Parameters**:
- `args` (array of strings): Arguments to pass to the west command

**Example Usage**:
```json
{
  "name": "west",
  "arguments": {
    "args": ["build", "-b", "native_sim"]
  }
}
```

### Error Handling

The server will return appropriate errors for:
- Blocked commands (flash, debug, etc.)
- Commands not in allowed list
- Execution failures
- Timeouts (5 minute limit)

## Best Practices

1. **Always verify changes**: Run `west build` after code modifications
2. **Use version control**: Commit logical, atomic changes
3. **Test thoroughly**: Use `west build -t run` to verify application behavior
4. **Follow conventions**: Match existing code style and structure
5. **Document**: Update docs when behavior changes

## Emergency Procedures

If the workspace gets into a bad state:

1. Check status: `west status`
2. Update dependencies: `west update`
3. Clean build: Remove `build/` directory and rebuild
4. Consult documentation: Check Zephyr docs for command help

## Contact

For questions or issues with the MCP server or agent rules, refer to the project README or open an issue.
