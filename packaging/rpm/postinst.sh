#!/bin/bash
# Post-installation script for devops-validator RPM package

echo "DevOps Validator installed successfully!"
echo "Run 'devops-validator help' to get started."

# Create symlink if needed
if [ ! -f /usr/bin/devops-validator ]; then
    ln -sf /usr/local/bin/devops-validator /usr/bin/devops-validator 2>/dev/null || true
fi

exit 0
