#!/usr/bin/env python3

from typing import Any, Iterable, Pattern, Tuple

try:
    from termcolor import colored, cprint
except ImportError:
    print("Install termcolor:")
    print("!!!!!!! pip3 install termcolor")
    print()

    def colored(
            text: str, color: str | None = ...,
            on_color: str | None = ...,
            attrs: Iterable[str] | None = ...,
            *, no_color: bool | None = ...,
            force_color: bool | None = ...) -> str:
        return text

    def cprint(
            text: str, color: str | None = ...,
            on_color: str | None = ...,
            attrs: Iterable[str] | None = ..., *,
            no_color: bool | None = ...,
            force_color: bool | None = ...,
            **kwargs: Any) -> None:
        print(text, **kwargs)

try:
    import click
except ImportError:
    print(colored("Install termcolor:", 'red'))
    print(colored("!!!!!!! pip3 install click", 'red'))
    print()

try:
    from psutil import virtual_memory
except ImportError:
    print(colored("Install psutil:", 'red'))
    print(colored("!!!!!!! pip3 install psutil", 'red'))
    print()

    class svmem(dict):
        pass

    def virtual_memory() -> svmem:
        d = svmem()
        d.total = 0
        return d

import json
import logging
import os
import platform
import re
import subprocess


def preambula(name: str):
    len_eq = 70 - len(name)
    print('='*(len_eq // 2), end=' ')
    cprint(name, attrs=["bold"], end=' ')
    print('='*(len_eq - len_eq // 2))


def get_regexp_for_version(name: str) -> Pattern[str]:
    if name in {"make"}:
        return re.compile(r' ([0-9]+\.[0-9]+)')
    else:
        return re.compile(r' ([0-9]+\.[0-9]+\.[0-9]+)')


def get_version(name: str):
    app_version = subprocess.run([name, "--version"], capture_output=True)
    version_out = app_version.stdout.decode("utf-8")
    p: Pattern[str] = get_regexp_for_version(name)
    nums_versions = list(set(p.findall(version_out)))
    if len(nums_versions) != 1:
        print(colored('Contact to @sokolik and show this output', 'red'))
        print(nums_versions)
    if not nums_versions:
        raise Exception("We ddidn't find version!")

    major_version = tuple(int(e) for e in nums_versions[0].split('.'))
    return (major_version, version_out)


def check_not_default(name: str, min_version, how_to_fix: str):
    try:
        major_version, version_out = get_version(name)

        if major_version < min_version:
            return False

        print(colored("You have correct version {}, but it's not default".format(name), 'red'))
        if how_to_fix:
            print(colored("Apply the next command, to fix problem", 'yellow'))
            print(colored(how_to_fix, 'yellow'))
        return True
    except Exception:
        return False


def print_version(
        name: str,
        min_version: Tuple[int],
        how_to_install: str = '',
        not_def_name: str = '',
        how_to_change_def: str = '') -> None:
    preambula(name)

    is_all_good = True
    is_not_installed = False
    is_not_updated = False

    try:
        major_version, version_out = get_version(name)

        color_to_print = 'green'
        if major_version < min_version:
            is_not_updated = True
            is_all_good = False
            color_to_print = 'red'

        print(colored(version_out, color_to_print))
    except Exception as e:
        is_not_installed = True
        is_all_good = False
        print(colored('Warning!!! ' + str(e), 'red'))
        print()

    if not is_all_good and not_def_name:
        is_not_default = check_not_default(not_def_name, min_version, how_to_change_def.format(name, not_def_name))
        if is_not_default:
            is_not_updated = False
            is_not_installed = False

    if is_not_installed or is_not_updated:
        print(colored(
            "You must install or update it! minimal_version={}".format(
                '.'.join(map(str,min_version))), 'yellow'))
        print(colored(how_to_install, 'yellow'))

        # TODO(sokolik): add how to update some apps
        # if is_not_updated:
        #     print(colored(how_to_update, 'yellow'))


def getSystemInfo():
    try:
        info = {}
        info['platform'] = platform.system()
        info['platform-release'] = platform.release()
        info['platform-version'] = platform.version()
        info['architecture'] = platform.machine()
        # info['hostname']=socket.gethostname()
        # info['ip-address']=socket.gethostbyname(socket.gethostname())
        # info['mac-address']=':'.join(re.findall('..', '%012x' % uuid.getnode()))
        info['processor'] = platform.processor()
        info['ram'] = str(round(virtual_memory().total / (1024.0 ** 3))) + " GB"
        return json.dumps(info)
    except Exception as e:
        logging.exception(e)


@click.command
@click.option('--all-info', is_flag=True, default=False, help='Print all possible warnins')
def main(all_info: bool) -> None:
    how_to_fix_default = """
    sudo rm -rf /usr/bin/{0} && sudo ln -s /usr/bin/{1} /usr/bin/{0} && {0} --version
    """
    print_version('python3', (3, 11, 0), 'python3.11', how_to_fix_default, '')

    clang_version = (17, 0, 0)
    how_to_install_clang = """
    wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && sudo ./llvm.sh {} all
    """.format(clang_version[0])
    print_version('clang', clang_version, how_to_install_clang, 'clang-17', how_to_fix_default)
    print_version('clang++', clang_version, how_to_install_clang, 'clang++-17', how_to_fix_default)
    print_version('clang-format', clang_version, how_to_install_clang, 'clang-format-17', how_to_fix_default)
    print_version('clang-tidy', clang_version, how_to_install_clang, 'clang-tidy-17', how_to_fix_default)

    gcc_version = (13, 0, 0)
    how_to_install_gcc = """
    sudo apt-get install g++-{}
    """.format(clang_version[0])
    print_version('gcc', gcc_version, how_to_install_gcc, 'gcc-13', how_to_fix_default)
    print_version('g++', gcc_version, how_to_install_gcc, 'g++-13', how_to_fix_default)

    print_version('bazel', (6, 4, 0), 'Read https://bazel.build/install/ubuntu')
    print_version('cmake', (3, 24, 0), 'sudo apt install cmake')
    print_version('make', (3, 81), 'sudo apt install build-essential manpages-dev git automake', '')

    print_version('git', (2, 40, 0), 'sudo apt install git')

    if all_info:
        print_version('lldb', clang_version, how_to_install_clang, 'lldb-17', how_to_fix_default)

        sys_info = json.loads(getSystemInfo())
        preambula('sys_info')
        print(colored(json.dumps(sys_info, indent=4), "yellow"))

        try:
            raise ImportError
            import git
            repo = git.Repo(os.getcwd())
            print(colored('Branch rep is ' + str(repo.head.reference), "yellow"))
            print(colored('Commit rep is ' + str(repo.head.object.hexsha), "yellow"))
        except ImportError:
            print(colored("Install gitpython:", 'red'))
            print(colored("!!!!!!! pip3 install GitPython", 'red'))
            print()

        # TODO(sokolik): add boost check
        # boost_vers = subprocess.run(['dpkg', '-s', 'libboost-dev', '|', 'grep', "'Version'"], capture_output=True)
        # print(boost_vers.stdout)
        # mac-os: brew update && brew upgrade


if __name__ == '__main__':  # pragma: nocover
    main()
