# MOLLER Experiment JANA2 Application

This project is a JANA2-based application for processing MOLLER experiment data using EVIO format input files. The application includes custom event sources, processors, and factories for handling FADC250 detector data.

## Dependencies

This project requires two main dependencies that must be installed separately:

### 1. JANA2 Framework

JANA2 is the core framework used by this application. You need to build and install JANA2 before building this project.

#### Building JANA2

```bash
git clone https://github.com/JeffersonLab/JANA2.git JANA2
cd JANA2

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=`pwd`
cmake --build build --target install -j 20
```

### 2. EVIO Library

The EVIO library is required for reading EVIO format data files.

#### Building EVIO

```bash
git clone --branch evio-6-dev https://github.com/JeffersonLab/evio/
cd evio
cmake -S . -B build
cmake --build build --target install --parallel
```

## Building the MOLLER Application

### Prerequisites

* CMake 3.16 or higher
* C++20 compatible compiler
* Built and installed JANA2 (see above)
* Built EVIO library (see above)

### Build Instructions

1. **Configure the build:**

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/JANA2;/path/to/EVIO"
```

* Replace `/path/to/JANA2` with the path to your JANA2 installation directory.
* Replace `/path/to/EVIO` with the path to your EVIO installation directory (e.g., the Linux-x86\_64 folder).

2. **Build the application:**

```bash
cmake --build build --parallel
```

## Project Structure

* `moller.cc` – Main application entry point
* `JEventSource_EVIO.cc/.h` – Custom EVIO event source
* `JEventProcessor_MOLLER.cc/.h` – Main event processor for MOLLER data
* `JFactory_FADC250.cc/.h` – Factory for FADC250 detector data
* `FADC250.h` – Data structure for FADC250 hits
* `EvioEventWrapper.h` – Wrapper for EVIO event data

## Usage

After building, run the application with:

```bash
./build/moller [jana_options] <evio_file1> [evio_file2] ...
```

The application will process the specified EVIO files and output results according to the configured processors.

## Build Types

* **Debug:** Add `-DCMAKE_BUILD_TYPE=Debug` to the `cmake` command for development and debugging.
* **Release:** Add `-DCMAKE_BUILD_TYPE=Release` for optimized production builds.

## Example Complete Build Sequence

```bash
# 1. Build JANA2
git clone https://github.com/JeffersonLab/JANA2.git JANA2
cd JANA2
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=`pwd`
cmake --build build --target install -j 20
cd ..

# 2. Build EVIO
git clone --branch evio-6-dev https://github.com/JeffersonLab/evio/
cd evio
cmake -S . -B build
cmake --build build --target install --parallel
cd ..

# 3. Build MOLLER application
cd moller_exp
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/JANA2;/path/to/EVIO"
cmake --build build --parallel
```
