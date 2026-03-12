# jana2-common-extensions

A collection of reusable [JANA2](https://jeffersonlab.github.io/JANA2/) plugins and libraries for reading and decoding EVIO-format data files produced by Jefferson Lab experiments. This repository can be extended for any experiment that uses EVIO-format readout with VME/VXS hardware modules.

## Table of Contents

- [Dependencies](#dependencies)
- [Build Instructions](#build-instructions)
- [Installation Layout](#installation-layout)
- [Using the Plugins with JANA2](#using-the-plugins-with-jana2)
- [Plugin Documentation](#plugin-documentation)

---
## Dependencies

| Dependency | Minimum Version | Notes |
|---|---|---|
| CMake | 3.16 | Build system |
| C++ compiler | C++20 | GCC 11+ or Clang 13+ recommended |
| [JANA2](https://github.com/JeffersonLab/JANA2) | 2.x | Event processing framework |
| [EVIO](https://github.com/JeffersonLab/evio) | v6.1.2 | Jefferson Lab EVIO C++ library |
| [ROOT](https://root.cern/install/) | 6.x | Data analysis and output |

### Building JANA2
```tcsh
git clone https://github.com/JeffersonLab/JANA2.git JANA2
cd JANA2
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=`pwd`
cmake --build build --target install -j`nproc`
cd ..
```

### Building EVIO
```tcsh
git clone https://github.com/JeffersonLab/evio/
cd evio
git checkout v6.1.2
cmake -S . -B build
cmake --build build --target install --parallel
cd ..
```

### ROOT

Install ROOT by following the official guide at <https://root.cern/install/>.

---

## Build Instructions

> **Important:** `CMAKE_INSTALL_PREFIX` must be set at **configure time** (the first `cmake` call). It is baked into generated headers (specifically `jce_config_paths.h`) so that config files such as `mapping.db` and `filter.db` are found correctly after installation. Changing it after the build without reconfiguring will produce incorrect paths.

### 1. Configure

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/JANA2;/path/to/evio;/path/to/root-6.32.20" -DCMAKE_INSTALL_PREFIX=/path/to/install
```

Replace the paths in `CMAKE_PREFIX_PATH` with the actual locations of your JANA2, EVIO, and ROOT **installations**.

Optional build-type flags:

```bash
# Debug build (adds -g, disables optimisation)
-DCMAKE_BUILD_TYPE=Debug

# Release build (full optimisation)
-DCMAKE_BUILD_TYPE=Release
```

### 2. Build

```bash
cmake --build build --parallel
```

### 3. Install

```bash
cmake --install build
```

---

## Installation Layout

After `cmake --install build` the install prefix will contain:

```
install/
├── config/
│   ├── mapping.db          # Bank-to-module ID mapping
│   └── filter.db           # ROC/bank allow-list (used when filtering is enabled)
├── include/
│   └── jce_config_paths.h  # Generated config-path resolver header
├── lib/
│   ├── cmake/
│   │   └── jana2_common_extensionsTargets.cmake
│   └── plugins/
│       ├── evio_parser.so
│       └── evio_processor.so
│       └── other_plugins.so
│       └── ....
└── templates/
    └── jce_config_paths.h.in
```

---

## Using the Plugins with JANA2

Point JANA2 to the installed plugin directory using the `JANA_PLUGIN_PATH` environment variable, then request the plugins by name with `-Pplugins=`.

### Basic usage

```bash
setenv JANA_PLUGIN_PATH ${JCE_INSTALL_PATH}/lib/plugins

jana -Pplugins=evio_parser,evio_processor /path/to/data.evio
```

### Combining with other plugin directories

```bash
setenv JANA_PLUGIN_PATH ${JCE_INSTALL_PATH}/lib/plugins:/my/custom/plugins

jana -Pplugins=evio_parser,evio_processor,my_custom_plugin /path/to/data.evio
```

### Running with ROC/bank filtering enabled

```bash
jana -Pplugins=evio_parser,evio_processor -PFILTER:ENABLE=1 -PFILTER:FILE=./install/config/filter.db /path/to/data.evio
```

For the full list of runtime parameters see the plugin READMEs linked below.

---

## Plugin Documentation

| Plugin | README |
|---|---|
| `evio_parser` | [plugins/evio_parser/README.md](plugins/evio_parser/README.md) |
| `evio_processor` | [plugins/evio_processor/README.md](plugins/evio_processor/README.md) |

