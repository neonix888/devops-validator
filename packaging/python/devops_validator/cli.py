#!/usr/bin/env python3
"""
CLI wrapper for devops-validator binary.
"""

import subprocess
import sys
import os

def find_binary():
    """Find the devops-validator binary."""
    # Check if it's in PATH
    from shutil import which
    binary = which("devops-validator")
    if binary:
        return binary

    # Check common installation locations
    possible_paths = [
        "/usr/local/bin/devops-validator",
        "/usr/bin/devops-validator",
        os.path.expanduser("~/.local/bin/devops-validator"),
    ]

    for path in possible_paths:
        if os.path.exists(path) and os.access(path, os.X_OK):
            return path

    raise FileNotFoundError(
        "devops-validator binary not found. Please install it first."
    )

def main():
    """Main entry point for the CLI."""
    try:
        binary = find_binary()
        result = subprocess.run([binary] + sys.argv[1:])
        sys.exit(result.returncode)
    except FileNotFoundError as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
