from pathlib import Path
import os
import platform
import subprocess
import sys

PROJECT_DIR = Path(__file__).parent.parent.resolve()
SCRIPTS_DIR = PROJECT_DIR/'scripts'
CONFIGURATIONS_DIR = PROJECT_DIR/'configurations'
PYTHON_EXECUTABLE = sys.executable
REPO_NAME = PROJECT_DIR.name

def program_available(program):
    command = f'{program} --version'
    result = subprocess.run(command, shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return result.returncode == 0

def run_command(command, **kwargs):
    print(f'Running command: {command}')
    result = subprocess.run(command, **kwargs)
    return result

def running_on_github_actions():
    return os.getenv('GITHUB_ACTIONS') == 'true'

def running_on_windows():
    return platform.system() == 'Windows'

def running_on_linux():
    return platform.system() == 'Linux'

def running_on_macos():
    return platform.system() == 'Darwin'

def running_on_unix():
    return os.name() == 'posix'
