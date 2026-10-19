# HallA-compton-jana2

This package reads Hall A Compton EVIO data and writes a ROOT file and a text
hit summary. It uses **jana2-common-extensions (JCE)** for the shared data-reading
framework and **ROOT** for the output file.

## Start here

| Your situation | What to do |
|---|---|
| JCE is already installed, but Compton is not built yet | [A. Build only Compton](#a-jce-is-already-installed) |
| JCE is not installed | [B. Build JCE and Compton together](#b-jce-is-not-installed) |

Before starting:

- Have ROOT 6 installed and loaded (`root-config` available on `PATH`).
- Have CMake and a C++20 compiler available. The full JCE build also needs Git,
  Boost and LZ4 development files, and internet access for dependency downloads.
- Run all commands below **from the `HallA-compton-jana2` folder**.

## A. JCE is already installed

Use this path to build Compton against an existing JCE installation without
rebuilding JCE, JANA2, or EVIO. The installation must provide the complete
JCE/JANA2/EVIO stack. For the default JCE build, its location is the
`jce-stack` folder inside `jana2-common-extensions`.

### 1. Set the JCE installation path

Bash:

```bash
export JCE_HOME="/path/to/jana2-common-extensions/jce-stack"
```

tcsh:

```tcsh
setenv JCE_HOME /path/to/jana2-common-extensions/jce-stack
```

### 2. Build and install Compton

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH="$JCE_HOME;`root-config --prefix`" -DCMAKE_INSTALL_PREFIX="$PWD/install"
cmake --build build --parallel
cmake --install build
```

This installs Compton into `install/`: plugins in `install/lib/plugins/`,
headers in `install/include/`, and configuration in `install/config/`.

### 3. Set up and run

Continue to [Set up replay environment](#set-up-replay-environment).
You do not need to set `JCE_SOURCE_DIR` for this build path.

**Do not use `source setup_compton.sh build ...` or `source setup_compton.csh build ...` to
skip the JCE build.** Their `build` mode always runs the JCE stack build before
building Compton, even when `JCE_HOME` points to an existing installation.

## B. JCE is not installed

Use the setup script to build and install the complete JCE stack, followed by
Compton. ROOT must already be installed; the script does not install ROOT.

### 1. Choose the JCE source location (Optional)

`JCE_SOURCE_DIR` is only needed when using the script's `build` mode with JCE
sources outside the default sibling folder. It is optional for the default
layout below.

Normally, leave `JCE_SOURCE_DIR` unset. The script looks for this layout and
downloads `jana2-common-extensions` if that sibling folder is missing:

```text
parent-folder/
├── HallA-compton-jana2/       ← run commands here
└── jana2-common-extensions/
    └── jce-stack/            ← installed JCE stack
```

If you already downloaded the JCE source somewhere else, select it first:

Bash:

```bash
export JCE_SOURCE_DIR="/path/to/jana2-common-extensions"
```

tcsh:

```tcsh
setenv JCE_SOURCE_DIR /path/to/jana2-common-extensions
```

An explicitly selected source folder must already exist and contain
`superbuild/CMakeLists.txt`. The script reuses existing sources without pulling
updates from Git.

### 2. Build and install

Bash:

```bash
source setup_compton.sh build
```

tcsh:

```tcsh
source setup_compton.csh build
```

The script checks ROOT, builds JCE's pinned JANA2/EVIO dependencies and JCE,
then builds and installs Compton.
By default, the stack is installed into `JCE_SOURCE_DIR/jce-stack`; Compton
is installed into `HallA-compton-jana2/install/`. If `JCE_HOME` is already set, the stack
will be installed there instead.

The script detects the loaded ROOT installation using `root-config --prefix`.
To select another installation, pass its prefix explicitly:
`source setup_compton.sh build /path/to/ROOT` (or `setup_compton.csh` for tcsh).
The script stops at the first failure. A successful build also sets up the
current terminal for replay. Continue to [Run replay](#run-replay).

## Set up replay environment

After installation, configure the environment using either the setup script or
manual settings below. ROOT should remain loaded in the current environment.

### Option 1: Use the setup script

From the `HallA-compton-jana2` folder:

Bash:

```bash
source setup_compton.sh
```

tcsh:

```tcsh
source setup_compton.csh
```

Without `build`, the script checks for the installed JCE and Compton packages
and configures these variables:

| Variable | Setup action |
|---|---|
| `JCE_HOME` | Uses the existing value, or defaults to the sibling JCE source folder's `jce-stack` installation |
| `COMPTON_HOME` | Sets it to this project's `install/` directory |
| `JANA_PLUGIN_PATH` | Prepends `$COMPTON_HOME/lib/plugins` |
| `JCE_CONFIG_DIR` | Appends `$COMPTON_HOME/config` |

Existing plugin and config paths are preserved, and repeated sourcing avoids
duplicate entries. For a custom JCE installation, set `JCE_HOME` before sourcing.
`JCE_SOURCE_DIR` is not required for setup only when `JCE_HOME` points to the
installed stack. If `JCE_SOURCE_DIR` is already set, the script validates it
even without `build`: correct it or unset it if the source folder no longer
exists. When `JCE_HOME` is unset, `JCE_SOURCE_DIR` controls its default location.

### Option 2: Set the environment manually

The setup script is optional. These settings provide the same runtime paths
without invoking it. Use the actual JCE and Compton installation locations.

Bash:

```bash
export JCE_HOME="/path/to/jana2-common-extensions/jce-stack"
export COMPTON_HOME="/path/to/HallA-compton-jana2/install"
export JANA_PLUGIN_PATH="$COMPTON_HOME/lib/plugins"
export JCE_CONFIG_DIR="$COMPTON_HOME/config"
```

tcsh:

```tcsh
setenv JCE_HOME /path/to/jana2-common-extensions/jce-stack
setenv COMPTON_HOME /path/to/HallA-compton-jana2/install
setenv JANA_PLUGIN_PATH "$COMPTON_HOME/lib/plugins"
setenv JCE_CONFIG_DIR "$COMPTON_HOME/config"
```

Apply them once per terminal to avoid duplicates.

### Run replay

Bash:

```bash
"${JCE_HOME}/scripts/jce.sh" -Pjana:nevents=10 /path/to/data.evio
```

tcsh:

```tcsh
"${JCE_HOME}/scripts/jce.csh" -Pjana:nevents=10 /path/to/data.evio
```

The replay command processes up to 10 events as a first check. Remove
`-Pjana:nevents=10` to process the full file. Compton's parser and processor are
loaded automatically through `$COMPTON_HOME/config/default_plugins.db`.

By default, output is written in the directory where you run replay:

- `evio_processor.root`: ROOT output.
- `evio_processor_hits.txt`: text hit summary.

**Existing ROOT output is overwritten.** Choose an output name for each run:

```sh
"${JCE_HOME}/scripts/jce.sh" -PROOT_OUT_FILENAME=run_042.root -PTXT_OUT_FILENAME=run_042_hits.txt /path/to/data.evio
```

For tcsh, use `jce.csh` in the same command.

## Which path variable should I use?

| Variable | What it points to | When you need it |
|---|---|---|
| `JCE_SOURCE_DIR` | JCE source code, containing `superbuild/` and `src/` | Set this when building from JCE sources outside the default sibling folder. To configure replay with an installed JCE stack, set `JCE_HOME` instead. |
| `JCE_HOME` | Installed JCE/JANA2/EVIO stack, containing `bin/jana` and `scripts/` | To use or install a stack outside the default `jce-stack` location |
| `COMPTON_HOME` | Compton installation folder (`HallA-compton-jana2/install`) | Set by the setup script or manually for runtime setup |

Setting `JCE_HOME` does not select the JCE source code used by `build` mode.
For a custom source location and custom installation location, set both
`JCE_SOURCE_DIR` and `JCE_HOME`.

## Troubleshooting

| Message or problem | What to check |
|---|---|
| `Run from the HallA-compton-jana2 source directory` | Change to this repository's folder before sourcing the setup script. |
| `ROOT not found` | Load ROOT so `root-config` is on `PATH`, or pass the ROOT installation prefix after `build`. |
| `ROOTConfig.cmake not found` | Check the supplied ROOT path. The script searches its `cmake/`, `lib/cmake/ROOT/`, `lib64/cmake/ROOT/`, and the folder itself. |
| `JCE_SOURCE_DIR must point to an existing ... source directory` | Correct the source path, or unset the variable to use the default sibling folder. |
| `JCE or Compton is not installed` | Check `JCE_HOME`. If only JCE is installed, build Compton using section A; otherwise use section B. |
| CMake cannot find `jana2_common_extensions` | Use the installation folder for `JCE_HOME`, rather than the source folder. Check that the JCE installation completed. |
| An earlier ROOT installation is still selected | CMake remembers paths in `build/`. Reconfigure with `-DROOT_DIR=/actual/folder/containing/ROOTConfig.cmake`, or use the script's build mode with the desired ROOT prefix. |

## Build settings and developer checks

The default build type is `RelWithDebInfo`: optimized processing with debug
symbols for profiling. Existing explicit choices in the CMake cache are
preserved. For a Compton-only build, choose another type at configuration time
with, for example, `-DCMAKE_BUILD_TYPE=Release`. The JCE superbuild forwards its
selected type to JANA2, EVIO, and JCE; rebuild and install after changing it.

Run `python3 tests/test_compton_setup.py` to check shell setup and build
orchestration. It uses temporary installations and a mock CMake command,
without downloading or building dependencies. For physics validation, replay
a known EVIO sample and check nonzero accumulator entries in `compton_tree`
together with expected raw-hit counts.

## Useful replay options

Add options to the replay command using `-P<name>=<value>`.

| Option | Default | Purpose |
|---|---|---|
| `ROOT_OUT_FILENAME` | `evio_processor.root` | ROOT output file name |
| `TXT_OUT_FILENAME` | `evio_processor_hits.txt` | Text hit-summary file name |
| `jana:nevents` | No limit specified | Maximum number of events to process |
