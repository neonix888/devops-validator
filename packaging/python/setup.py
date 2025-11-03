#!/usr/bin/env python3
"""
Python wrapper for devops-validator CLI tool.

This package downloads the appropriate binary for your platform
and provides a Python interface to the devops-validator tool.
"""

from setuptools import setup, find_packages
from setuptools.command.install import install
import platform
import urllib.request
import os
import stat

VERSION = "1.0.0"
REPO = "neonix888/devops-validator"

class BinaryInstall(install):
    """Custom installation to download platform-specific binary."""

    def run(self):
        install.run(self)
        self.download_binary()

    def download_binary(self):
        system = platform.system().lower()
        machine = platform.machine().lower()

        # Map to GitHub release asset names
        if system == "linux":
            if "x86_64" in machine or "amd64" in machine:
                asset = f"devops-validator-{VERSION}-Linux-x86_64.tar.gz"
            elif "aarch64" in machine or "arm64" in machine:
                asset = f"devops-validator-{VERSION}-Linux-aarch64.tar.gz"
            else:
                raise RuntimeError(f"Unsupported architecture: {machine}")
        elif system == "darwin":
            asset = f"devops-validator-{VERSION}-macOS.tar.gz"
        elif system == "windows":
            asset = f"devops-validator-{VERSION}-Windows.zip"
        else:
            raise RuntimeError(f"Unsupported platform: {system}")

        url = f"https://github.com/{REPO}/releases/download/v{VERSION}/{asset}"
        print(f"Downloading binary from: {url}")

        # Download would happen here in a real implementation
        # For now, this is a placeholder showing the concept

setup(
    name="devops-validator",
    version=VERSION,
    description="Multi-platform DevOps validation CLI tool",
    long_description=open("README.md").read() if os.path.exists("README.md") else "",
    long_description_content_type="text/markdown",
    author="neonix888",
    author_email="neonix888@github.com",
    url=f"https://github.com/{REPO}",
    packages=find_packages(),
    entry_points={
        "console_scripts": [
            "devops-validator=devops_validator.cli:main",
        ],
    },
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Topic :: Software Development :: Build Tools",
        "Topic :: System :: Systems Administration",
    ],
    python_requires=">=3.8",
    cmdclass={
        "install": BinaryInstall,
    },
)
