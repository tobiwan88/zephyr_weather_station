# Agent Rules for Zephyr Weather Station Development

## Overview

This document defines the rules and guidelines for AI agents working on the Zephyr Weather Station project.

## MCP Tools Available

The project provides an MCP (Model Context Protocol) server that exposes two guarded tools for safe command execution:

1. **west** - Zephyr build system commands
2. **git** - Version control commands

### West Tool Usage

The `west` tool is available for Zephyr-specific operations.

#### Allowed West Commands

The following `west` commands are **ALLOWED** and can be executed freely:

- `west build` - Build the application
- `west update` - Update dependencies
- `west status` - Check workspace status  
- `west twister` - Run test automation

#### Blocked West Commands

The following `west` commands are **BLOCKED** for security reasons:

- `west flash` - Flashing firmware to hardware (requires physical access)
- `west debug` - Starting debug sessions (requires hardware/emulator)
- `west debugserver` - Starting debug server
- `west attach` - Attaching to running processes

**Rationale**: These commands interact with physical hardware or require special privileges that should not be automated without explicit human oversight.

### Git Tool Usage

The `git` tool is available for version control operations, allowing agents to track progress and enable reviewers to see incremental changes.

#### Allowed Git Commands

The following `git` commands are **ALLOWED** and can be executed freely:

- `git add` - Stage files for commit
- `git commit` - Create commits with meaningful messages
- `git status` - Check repository status
- `git diff` - View changes
- `git log` - View commit history
- `git show` - Show commit details
- `git checkout` - Switch branches or restore files

#### Blocked Git Commands

The following `git` commands are **BLOCKED** for security reasons:

- `git push` - Pushing to remote (handled by report_progress tool)
- `git pull` - Pulling from remote
- `git fetch` - Fetching from remote
- `git reset` - Resetting commits (prevents history rewriting)
- `git rebase` - Rebasing commits (prevents history rewriting)
- `git merge` - Merging branches

**Rationale**: Remote operations and history-rewriting commands should be handled through approved workflows (like report_progress) to maintain audit trail and prevent accidental data loss.

#### Git Best Practices for Agents

1. **Commit frequently**: Create commits between logical steps to enable easy rollback
2. **Meaningful commit messages**: Use descriptive messages that explain what changed and why
   - Good: "Add git tool to MCP server for version control support"
   - Bad: "Update files" or "Fix bug"
3. **Stage selectively**: Use `git add <file>` to stage specific files rather than `git add .`
4. **Check status first**: Always run `git status` before committing to verify what will be committed
5. **Review diffs**: Use `git diff` to review changes before staging
6. **Small commits**: Keep commits focused on a single logical change

#### Example Workflow

```bash
# 1. Check current status
git status

# 2. Review changes
git diff path/to/file

# 3. Stage specific files
git add path/to/file1 path/to/file2

# 4. Create a meaningful commit
git commit -m "Add feature X: explanation of changes"

# 5. Verify commit
git log -1 --oneline
```

## Development Guidelines

### 1. Code Changes and Version Control

- **Use git for progress tracking**: Create commits between logical steps
- **Write meaningful commit messages**: Explain what changed and why
  - Include context about the change
  - Reference issue numbers if applicable
  - Use imperative mood (e.g., "Add feature" not "Added feature")
- **Commit frequently**: This allows reviewers to understand the progression of changes
- **Stage files selectively**: Use `git add <specific-files>` to control what gets committed
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

- **Never commit secrets or credentials**
  - API keys, tokens, and secrets must NEVER be committed to the repository
  - Use environment variables for sensitive data
  - The `.gitignore` is configured to block common secret files
- Follow secure coding practices
- Be cautious with external dependencies

## Cline AI Assistant Setup

This project is configured to work with the Cline AI assistant using OpenRouter API with Qwen3 235B model (free tier).

### Configuration Files

- **cline/cline_api_config.template.json**: Template configuration (safe to commit)
- **cline/cline_api_config.json**: Active configuration with API key (NEVER commit, auto-generated)
- **cline/cline_mcp_settings.json**: MCP server settings
- **cline/setup_cline.sh**: Automatic setup script

### Security Model

1. **Template-based configuration**: The `cline_api_config.template.json` contains all settings except the API key
2. **Runtime injection**: API key is provided via `OPENROUTER_API_KEY` environment variable when starting the container
3. **Auto-generation**: The setup script creates `cline_api_config.json` from the template and injects the API key
4. **Git protection**: `.gitignore` blocks `cline_api_config.json` and other secret files from being committed

### Usage

```bash
# Start container with API key
docker run -it -v $(pwd):/workspace -e OPENROUTER_API_KEY=sk-or-v1-xxx... zephyr_weather_station

# The setup_cline.sh script automatically:
# 1. Checks for OPENROUTER_API_KEY environment variable
# 2. Creates /cline/cline_api_config.json with the API key injected
# 3. Copies MCP settings to /cline/cline_mcp_settings.json
```

### Changing Models

To use a different OpenRouter model, edit `cline/cline_api_config.template.json`:

```json
{
  "openRouterModelId": "qwen/qwen3-235b-a22b:free"
}
```

Available models: https://openrouter.ai/models

## Workspace Structure

```
zephyr_weather_station/
├── app/                    # Application code (T1 workspace self path)
│   ├── CMakeLists.txt     # Build configuration
│   ├── prj.conf           # Project configuration
│   └── src/
│       └── main.c         # Main application
├── west.yml               # West manifest
├── west_mcp_server.py     # MCP server for guarded west and git access
├── cline/
│   ├── cline_api_config.template.json  # Cline API config template (safe)
│   ├── cline_mcp_settings.json         # MCP server configuration
│   └── setup_cline.sh                  # Setup script for runtime config
├── Dockerfile             # Docker development environment
├── .gitignore             # Excludes secrets and build artifacts
└── README.md              # Project documentation
```

**Note**: `cline/cline_api_config.json` is generated at runtime and never committed.

## MCP Server Usage

The `west_mcp_server.py` provides a JSON-RPC server that exposes both `west` and `git` tools through MCP.

### Tool Interfaces

#### West Tool

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

#### Git Tool

**Tool Name**: `git`

**Parameters**:
- `args` (array of strings): Arguments to pass to the git command

**Example Usage**:
```json
{
  "name": "git",
  "arguments": {
    "args": ["add", "app/src/main.c"]
  }
}
```

```json
{
  "name": "git",
  "arguments": {
    "args": ["commit", "-m", "Add sensor initialization to main.c"]
  }
}
```

### Error Handling

The server will return appropriate errors for:
- Blocked commands (west: flash, debug; git: push, pull, reset, etc.)
- Commands not in allowed list
- Execution failures
- Timeouts (5 minutes for west, 1 minute for git)

## Best Practices

1. **Always verify changes**: Run `west build` after code modifications
2. **Use version control effectively**: 
   - Commit logical, atomic changes with meaningful messages
   - Use git to track progress and enable easy rollback
   - Stage files selectively before committing
3. **Test thoroughly**: Use `west build -t run` to verify application behavior
4. **Follow conventions**: Match existing code style and structure
5. **Document**: Update docs when behavior changes
6. **Enable reviewers**: Create commits between steps so reviewers can see the progression of changes

## Emergency Procedures

If the workspace gets into a bad state:

1. Check status: `west status`
2. Update dependencies: `west update`
3. Clean build: Remove `build/` directory and rebuild
4. Consult documentation: Check Zephyr docs for command help

## Contact

For questions or issues with the MCP server or agent rules, refer to the project README or open an issue.
