#!/usr/bin/env node

const { spawn } = require('child_process');
const path = require('path');
const os = require('os');

function findBinary() {
  const platform = os.platform();
  const ext = platform === 'win32' ? '.exe' : '';

  // Look for binary in package directory
  const binaryPath = path.join(__dirname, '..', 'bin', `devops-validator${ext}`);

  // In a real implementation, this would check if the binary exists
  // and fallback to system-installed version

  return 'devops-validator'; // Assume it's in PATH
}

const binary = findBinary();
const args = process.argv.slice(2);

const child = spawn(binary, args, {
  stdio: 'inherit',
  shell: true
});

child.on('exit', (code) => {
  process.exit(code || 0);
});

child.on('error', (err) => {
  console.error('Failed to execute devops-validator:', err.message);
  process.exit(1);
});
