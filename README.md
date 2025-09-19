# FADC250 Data Processing Application

A JANA2-based application for processing FADC250 detector data from EVIO-format files. For now, the application supports extracting and decoding FADC250 data only if each EVIO event contains exactly one detector event.

## Overview

This application provides the following pipeline for processing FADC250 data:
- EVIO file reading and event parsing
- Run control event detection and run number extraction
- FADC250 hit extraction (both waveform and pulse data)
- Structured data output for analysis

## Hardware Configuration

This application was developed and tested using data from the following hardware configuration:

- **FADC250 Module**: 1 unit at crate 1, slot 3
- **Data Bank ID**: 250
- **Operating Mode**: 10

## Prerequisites

Before building this application, ensure you have:

- CMake 3.16 or higher
- C++20 compatible compiler
- Git (for cloning dependencies)

## Dependencies

This project requires two main dependencies that must be built separately:

### 1. JANA2 Framework

JANA2 provides the core event processing framework for this application.

#### Building JANA2

```bash
git clone https://github.com/JeffersonLab/JANA2.git JANA2
cd JANA2
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=`pwd`
cmake --build build --target install -j 20
```

### 2. EVIO Library

The EVIO library is required for reading and parsing EVIO format data files.

#### Building EVIO

```bash
git clone https://github.com/JeffersonLab/evio/
cd evio
git checkout v6.1.2
cmake -S . -B build
cmake --build build --target install --parallel
```

## Building the Application

### Build Instructions

1. **Configure the build:**

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/JANA2;/path/to/EVIO/Linux-x86_64"
```

* Replace `/path/to/JANA2` with the path to your JANA2 directory.
* Replace `/path/to/EVIO` with the path to your EVIO directory.

2. **Build the application:**

```bash
cmake --build build --parallel
```

### Build Types

- **Debug:** Add `-DCMAKE_BUILD_TYPE=Debug` to the cmake command for development and debugging.
- **Release:** Add `-DCMAKE_BUILD_TYPE=Release` for optimized production builds.

### Complete Build Sequence

For a complete build from scratch:

```bash
# 1. Build JANA2
git clone https://github.com/JeffersonLab/JANA2.git JANA2
cd JANA2
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=`pwd`
cmake --build build --target install -j 20
cd ..

# 2. Build EVIO
git clone https://github.com/JeffersonLab/evio/
cd evio
git checkout v6.1.2
cmake -S . -B build
cmake --build build --target install --parallel
cd ..

# 3. Build application
cd moller_exp
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/JANA2;/path/to/EVIO/Linux-x86_64"
cmake --build build --parallel
```

## Usage

After building, run the application with:

```bash
./build/moller [jana_options] <evio_file1> [evio_file2] ...
```

The application will process the specified EVIO files and output detector hit information to `output.txt`.

### Customizing Output Filename

You can specify a custom output filename using the `OUTPUT_FILENAME` parameter:

```bash
./build/moller -POUTPUT_FILENAME=my_results.txt <evio_file1> [evio_file2] ...

```

## Project Structure

### Core Application Files
- `moller.cc` – Main application entry point
- `JEventSource_EVIO.cc/.h` – Custom EVIO event source
- `JEventProcessor_MOLLER.cc/.h` – Main event processor
- `JFactory_FADC250.cc/.h` – Factory for FADC250 detector data

### Data Objects (`data_objects/`)
- `EventHits.h` – Container for all event hits
- `EvioEventWrapper.h` – Wrapper for EVIO event data
- `FADC250Hit.h` – Base class for all FADC250 hits
- `FADC250PulseHit.h` – Pulse hit data structure
- `FADC250WaveformHit.h` – Waveform hit data structure

### Parser Utilities (`parsers/`)
- `EvioEventParser.cc/.h` – EVIO event parsing utilities
- `RawDataParser.cc/.h` – Raw data parsing utilities