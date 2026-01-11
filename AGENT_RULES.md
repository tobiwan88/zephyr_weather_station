# Agent Rules (Weather Station)

## Cline AI Agent Rules
- Always use Cline tools for file operations (read_file, write_to_file, replace_in_file)
- Use execute_command with requires_approval=true for potentially impactful operations
- Follow the tool usage guidelines in the Cline documentation
- Use task_progress parameter to track implementation progress

## Command policy
- Use MCP tool `west(args)` for all Zephyr actions (build/test/sca).
- **Board Target**: Always use `native_sim/native/64` for native simulation development
- **Build Location**: Execute west build from workspace root: `west build zephyr_weather_station/app -b native_sim/native/64 --pristine`
- Default tests: west ["twister","-T","tests","-p","native_sim/native/64"] (use native_sim/native/64 for quick iteration)
- Run static analysis regularly: west ["sca"] (or your repo's sca command)
- Do not run device commands (flash/debug) unless ALLOW_DEVICE_CMDS=1.
- Always check west.yml and app/prj.conf for required dependencies before adding new features.

## Zephyr Development Best Practices
- Follow Zephyr coding guidelines (snake_case, proper includes, SPDX headers)
- Use Zephyr APIs consistently:
  - zbus for inter-subsystem communication
  - Device Tree for hardware configuration
  - Kconfig for feature configuration
  - Logging subsystem for debug output
- Structure code using Zephyr subsystem patterns:
  - Clear initialization order
  - Proper error handling with Zephyr error codes
  - Thread-safe operations using Zephyr synchronization primitives
- Always add required Kconfig symbols to prj.conf
- Use devicetree overlays for board-specific configurations

## Cline Implementation Patterns
- Use replace_in_file for targeted code changes
- Use write_to_file for new file creation
- Always include complete file content when using write_to_file
- Use search_files for cross-file pattern analysis
- Use list_code_definition_names for architecture understanding
- Use browser_action for web-based testing and validation

## Development policy
- Implement in milestone-sized changes (refer to PLAN.md).
- Keep diffs small; no broad refactors without explicit need.
- After any functional change: update docs (VISION/ARCH/PLAN) if relevant.
- Follow the subsystem architecture defined in Architecture.md
- Test each milestone thoroughly before proceeding to the next

## Cline Development Workflow
- Start each session by reading relevant files to understand context
- Use task_progress to track implementation steps
- Make incremental changes with frequent validation
- Use attempt_completion when milestone is reached
- Always provide clear, technical explanations of changes

## Quality gates
- Ensure shell commands exist for new features (follow Zephyr shell subsystem patterns).
- Add/extend unit tests for each feature using Zephyr's testing framework.
- Ensure twister + sca can be run with a single documented command.
- Verify native_sim builds and runs without warnings
- Check memory usage and stack sizes are appropriate
- Validate zbus message integrity and thread safety

## Cline Quality Assurance
- Use execute_command to run build and test commands
- Validate changes with west build and west twister
- Use search_files to ensure consistent patterns across codebase
- Use list_code_definition_names to verify architecture compliance
- Always test changes on native_sim before proceeding

## Code Organization
- Place subsystem code in src/subsystems/ directory
- Use include/ directory for public headers
- Keep tests in tests/ directory with clear naming
- Follow Zephyr file naming conventions (snake_case.c)
- Group related functionality in logical modules

## Cline Code Management
- Use write_to_file for new subsystem creation
- Use replace_in_file for existing subsystem modifications
- Use search_files to find related code patterns
- Use list_files to understand project structure
- Always maintain proper SPDX headers and copyright notices

## Debugging and Logging
- Use Zephyr logging subsystem instead of printf for debug output
- Define appropriate log levels for each subsystem
- Include meaningful context in log messages
- Use shell commands for runtime inspection and debugging

## Cline Debugging Patterns
- Use execute_command to run debug builds with LOG_LEVEL_DBG
- Use search_files to find logging patterns across subsystems
- Use replace_in_file to add debug logging when needed
- Use browser_action for web-based debugging interfaces
- Always remove debug code before finalizing implementation