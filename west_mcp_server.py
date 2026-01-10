#!/usr/bin/env python3
"""
MCP Server for Zephyr West Tool
Provides a guarded west command execution with security restrictions.
"""

import json
import sys
import subprocess
from typing import Any, Sequence


class WestMCPServer:
    """MCP Server that exposes a guarded west tool."""
    
    # Allowed west commands
    ALLOWED_COMMANDS = {"build", "update", "status", "twister"}
    
    # Blocked west commands (for security)
    BLOCKED_COMMANDS = {"flash", "debug", "debugserver", "attach"}
    
    def __init__(self):
        self.tools = [
            {
                "name": "west",
                "description": "Execute west commands with security restrictions. Allowed: build, update, status, twister. Blocked: flash, debug.",
                "inputSchema": {
                    "type": "object",
                    "properties": {
                        "args": {
                            "type": "array",
                            "items": {"type": "string"},
                            "description": "Arguments to pass to west command"
                        }
                    },
                    "required": ["args"]
                }
            }
        ]
    
    def handle_initialize(self, params: dict) -> dict:
        """Handle initialize request."""
        return {
            "protocolVersion": "2024-11-05",
            "capabilities": {
                "tools": {}
            },
            "serverInfo": {
                "name": "west-mcp-server",
                "version": "1.0.0"
            }
        }
    
    def handle_tools_list(self, params: dict) -> dict:
        """Handle tools/list request."""
        return {"tools": self.tools}
    
    def handle_tools_call(self, params: dict) -> dict:
        """Handle tools/call request."""
        tool_name = params.get("name")
        arguments = params.get("arguments", {})
        
        if tool_name != "west":
            return {
                "content": [
                    {
                        "type": "text",
                        "text": f"Error: Unknown tool '{tool_name}'"
                    }
                ],
                "isError": True
            }
        
        return self.execute_west(arguments.get("args", []))
    
    def execute_west(self, args: list) -> dict:
        """Execute west command with security checks."""
        if not args:
            return {
                "content": [
                    {
                        "type": "text",
                        "text": "Error: No arguments provided to west command"
                    }
                ],
                "isError": True
            }
        
        # Check if the first argument is a blocked command
        command = args[0]
        if command in self.BLOCKED_COMMANDS:
            return {
                "content": [
                    {
                        "type": "text",
                        "text": f"Error: Command 'west {command}' is blocked for security reasons. Blocked commands: {', '.join(self.BLOCKED_COMMANDS)}"
                    }
                ],
                "isError": True
            }
        
        # Check if the command is in allowed list (if not a help or version command)
        if command not in self.ALLOWED_COMMANDS and command not in ["--help", "-h", "--version", "-v", "help"]:
            return {
                "content": [
                    {
                        "type": "text",
                        "text": f"Error: Command 'west {command}' is not in the allowed list. Allowed commands: {', '.join(self.ALLOWED_COMMANDS)}"
                    }
                ],
                "isError": True
            }
        
        # Execute the west command
        try:
            result = subprocess.run(
                ["west"] + args,
                capture_output=True,
                text=True,
                timeout=300  # 5 minute timeout
            )
            
            output = result.stdout
            if result.stderr:
                output += "\n" + result.stderr
            
            return {
                "content": [
                    {
                        "type": "text",
                        "text": output if output else "Command executed successfully with no output"
                    }
                ],
                "isError": result.returncode != 0
            }
        except subprocess.TimeoutExpired:
            return {
                "content": [
                    {
                        "type": "text",
                        "text": "Error: Command timed out after 5 minutes"
                    }
                ],
                "isError": True
            }
        except Exception as e:
            return {
                "content": [
                    {
                        "type": "text",
                        "text": f"Error executing west command: {str(e)}"
                    }
                ],
                "isError": True
            }
    
    def handle_request(self, request: dict) -> dict:
        """Handle incoming JSON-RPC request."""
        method = request.get("method")
        params = request.get("params", {})
        
        if method == "initialize":
            result = self.handle_initialize(params)
        elif method == "tools/list":
            result = self.handle_tools_list(params)
        elif method == "tools/call":
            result = self.handle_tools_call(params)
        else:
            result = {"error": {"code": -32601, "message": f"Method not found: {method}"}}
        
        return result
    
    def run(self):
        """Run the MCP server (stdio transport)."""
        for line in sys.stdin:
            try:
                request = json.loads(line)
                response = self.handle_request(request)
                
                # Add id to response if present in request
                if "id" in request:
                    response["id"] = request["id"]
                
                response["jsonrpc"] = "2.0"
                print(json.dumps(response), flush=True)
            except json.JSONDecodeError as e:
                error_response = {
                    "jsonrpc": "2.0",
                    "error": {"code": -32700, "message": f"Parse error: {str(e)}"},
                    "id": None
                }
                print(json.dumps(error_response), flush=True)
            except Exception as e:
                error_response = {
                    "jsonrpc": "2.0",
                    "error": {"code": -32603, "message": f"Internal error: {str(e)}"},
                    "id": request.get("id") if 'request' in locals() else None
                }
                print(json.dumps(error_response), flush=True)


if __name__ == "__main__":
    server = WestMCPServer()
    server.run()
