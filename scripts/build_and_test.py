import argparse
import json
import subprocess
import utils

BUILD_TYPES = ['debug-sanitized', 'debug-nonsanitized', 'release-sanitized', 'release-nonsanitized']

parser = argparse.ArgumentParser(description='Build and test C/C++ code')
parser.add_argument('-b', '--build-type', type=str, choices=BUILD_TYPES, help='Type of the build', default=None)
args = parser.parse_args()

if args.build_type:
    requested_build_types = [args.build_type]
else:
    requested_build_types = BUILD_TYPES

print(f'{requested_build_types=:}\n', flush=True)

for build_type in requested_build_types:
    command = f'cmake --preset config-{build_type}'
    utils.run_command(command, shell=True, check=True)

    command = f'cmake --build --preset build-{build_type}'
    utils.run_command(command, shell=True, check=True)

    command = f'ctest --preset test-{build_type}'
    utils.run_command(command, shell=True, check=True)

    if 'nonsanitized' in build_type:
        if utils.program_available('valgrind'):
            command = f'ctest --preset test-{build_type} --show-only=json-v1'
            result = utils.run_command(command, shell=True, check=True, stdout=subprocess.PIPE, text=True)

            tests_json = json.loads(result.stdout)
            tests = tests_json.get('tests', [])
            tests_executables_dict = {}  # Using dictionary and not set as iterating it preserves insertion order
            for test in tests:
                command = test.get('command', [])
                if command:
                    tests_executables_dict[command[0]] = None
                else:
                    print(f"No commands found for test {test.get('name')}")
                    exit(1)
            for key in tests_executables_dict:
                command = f'valgrind --error-exitcode=1 --leak-check=full {key}'
                result = utils.run_command(command, shell=True, check=True)

        if utils.program_available('clang-tidy') and not (utils.running_on_linux() and utils.running_on_github_actions()):
            command = f'clang-tidy --version'
            result = utils.run_command(command, shell=True, check=True)

            compilation_database_path = utils.PROJECT_DIR/'build'/build_type/'compile_commands.json'
            script = utils.SCRIPTS_DIR/'run_clang_tidy.py'
            command = f'{utils.PYTHON_EXECUTABLE} {script} -d {compilation_database_path} -e'
            result = utils.run_command(command, shell=True, check=True)
