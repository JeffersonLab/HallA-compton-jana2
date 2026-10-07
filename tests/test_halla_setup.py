"""Check sourced setup and build orchestration without downloading dependencies."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

source = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix="halla setup ") as temporary:
    temporary = str(Path(temporary).resolve())
    root = Path(temporary) / "Compton's checkout"
    jce = Path(temporary) / "jce"
    root.mkdir()
    for script in ("halla.sh", "halla.csh"):
        shutil.copyfile(source / script, root / script)
    (root / "CMakeLists.txt").touch()
    (root / "lib/plugins").mkdir(parents=True)
    (root / "config").mkdir(parents=True)
    (jce / "superbuild").mkdir(parents=True)
    (jce / "superbuild/CMakeLists.txt").touch()
    (jce / "jce-stack/bin").mkdir(parents=True)
    jana = jce / "jce-stack/bin/jana"
    jana.touch()
    jana.chmod(0o755)
    mockbin = Path(temporary) / "mockbin"
    mockbin.mkdir()
    cmake = mockbin / "cmake"
    cmake.write_text('#!/bin/sh\nprintf "%s\\n" "$*" >> "$HALLA_TEST_LOG"\nexit "${HALLA_TEST_FAIL:-0}"\n')
    cmake.chmod(0o755)
    git = mockbin / "git"
    git.write_text('#!/bin/sh\nprintf "%s\\n" "$*" >> "$HALLA_TEST_GIT_LOG"\n'
                   '[ "${HALLA_TEST_GIT_FAIL:-0}" = 0 ] || exit 1\n'
                   'cp -R "$HALLA_TEST_JCE_TEMPLATE" "$3"\n')
    git.chmod(0o755)
    env = {k: v for k, v in os.environ.items() if k not in ("JCE_HOME", "JCE_SOURCE_DIR")}
    env.update(JCE_SOURCE_DIR=str(jce), JCE_CONFIG_DIR="/other/config",
               JANA_PLUGIN_PATH="/other/plugins", PATH=f"{mockbin}:{env['PATH']}",
               CMAKE_PREFIX_PATH="/root/prefix:/other/prefix",
               HALLA_TEST_LOG=str(Path(temporary) / "build.log"))
    for shell in ("bash", "tcsh"):
        if not shutil.which(shell):
            print(f"SKIP: {shell} unavailable")
            continue
        invocation = [shell] + (["-f"] if shell == "tcsh" else []) + ["-c"]
        script = "halla.sh" if shell == "bash" else "halla.csh"
        command = f'source {script}; source {script}; env'
        result = subprocess.run(invocation + [command], cwd=root, env=env,
                                text=True, capture_output=True)
        assert result.returncode == 0, result.stderr
        settings = dict(line.split("=", 1) for line in result.stdout.splitlines() if "=" in line)
        assert settings["JCE_HOME"] == str(jce / "jce-stack")
        assert settings["COMPTON_HOME"] == str(root)
        assert settings["JANA_PLUGIN_PATH"] == f"{root}/lib/plugins:/other/plugins"
        assert settings["JCE_CONFIG_DIR"] == f"/other/config:{root}/config"
        root_prefix = Path(temporary) / "ROOT prefix"
        root_prefix.mkdir(exist_ok=True)
        (root_prefix / "cmake").mkdir(exist_ok=True)
        (root_prefix / "cmake/ROOTConfig.cmake").touch()
        log = Path(env["HALLA_TEST_LOG"])
        log.write_text("")
        result = subprocess.run(invocation + [f'source {script} build "{root_prefix}"'], cwd=root,
                                env=env, text=True, capture_output=True)
        assert result.returncode == 0, result.stderr
        assert len(log.read_text().splitlines()) == 5, (shell, log.read_text(), result.stdout, result.stderr)
        assert f";{root_prefix};/root/prefix;/other/prefix" in log.read_text()
        assert f"-DCMAKE_INSTALL_PREFIX={root}" in log.read_text()
        assert f"-DROOT_DIR={root_prefix}/cmake" in log.read_text()
        log.write_text("")
        result = subprocess.run(invocation + [f'source {script} build "{root_prefix}"'],
                                cwd=root, env=env, text=True, capture_output=True)
        assert result.returncode == 0, result.stderr
        assert f";{root_prefix};/root/prefix;/other/prefix" in log.read_text()
        assert len(log.read_text().splitlines()) == 5

        log.write_text("")
        result = subprocess.run(invocation + [f'source {script} build "{root_prefix}"'], cwd=root,
                                env=env | {"HALLA_TEST_FAIL": "1"}, text=True, capture_output=True)
        assert result.returncode != 0, result
        assert len(log.read_text().splitlines()) == 1, log.read_text()
        log.write_text("")
        result = subprocess.run(invocation + [f'source {script} build'], cwd=root,
                                env=env, text=True, capture_output=True)
        assert result.returncode != 0 and "ROOT prefix is required" in result.stderr
        assert log.read_text() == ""
        result = subprocess.run(invocation + [f'source {script} invalid'], cwd=root,
                                env=env, text=True, capture_output=True)
        assert result.returncode != 0 and "Usage" in result.stderr, result
        sibling = root.parent / "jana2-common-extensions"
        git_log = root.parent / "git.log"
        git_log.write_text("")
        clone_env = {k: v for k, v in env.items() if k != "JCE_SOURCE_DIR"}
        clone_env.update(HALLA_TEST_GIT_LOG=str(git_log), HALLA_TEST_JCE_TEMPLATE=str(jce))
        log.write_text("")
        result = subprocess.run(invocation + [f'source {script}'], cwd=root,
                                env=clone_env, text=True, capture_output=True)
        assert result.returncode != 0 and git_log.read_text() == ""
        for override in ("", str(root.parent / "missing-override")):
            result = subprocess.run(invocation + [f'source {script} build "{root_prefix}"'],
                                    cwd=root, env=clone_env | {"JCE_SOURCE_DIR": override},
                                    text=True, capture_output=True)
            assert result.returncode != 0 and git_log.read_text() == ""
        result = subprocess.run(invocation + [f'source {script} build "{root_prefix}"'],
                                cwd=root, env=clone_env | {"HALLA_TEST_GIT_FAIL": "1"},
                                text=True, capture_output=True)
        assert result.returncode != 0 and log.read_text() == ""
        git_log.write_text("")
        for _ in range(2):
            result = subprocess.run(invocation + [f'source {script} build "{root_prefix}"'],
                                    cwd=root, env=clone_env, text=True, capture_output=True)
            assert result.returncode == 0, result.stderr
        assert len(git_log.read_text().splitlines()) == 1, git_log.read_text()
        assert len(log.read_text().splitlines()) == 10, log.read_text()
        shutil.rmtree(sibling)
        print(f"PASS: {shell} setup, repeated source, build, failure handling")
