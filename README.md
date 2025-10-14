# FADC250 Data Processing Application

A JANA2-based application for processing FADC250 detector data from EVIO-format files. For now, the application supports extracting and decoding FADC250 data only if each EVIO event contains exactly one detector event.

## Data Processing Flow

1. **Event Source** reads EVIO files, extracts run numbers from control events (skipping them), and adds physics events to the processing queue
2. **Factory** fetches these events and passes them to the EVIO event parser 
3. **EVIO Event Parser** does EVIO event level parsing and delegates raw data parsing to Raw Data Parser
4. **Raw Data Parser** parses raw data and creates hit objects with extracted data
5. **Processor** consumes these hit objects and outputs the results to a ROOT file

## Hardware Configuration

This application was developed and tested using data from following:

- **FADC250 Module**: 1 unit at crate 1, slot 3
- **Data Bank ID**: 250
- **Operating Mode**: 10

## Prerequisites

Before building this application, ensure you have:

- CMake 3.16 or higher
- C++20 compatible compiler
- Git (for cloning dependencies)
- ROOT 6.3x.x (for data analysis and visualization)

## Dependencies

This project requires the following dependencies:

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

### 3. Building ROOT

**For ifarm users**: Use the pre-built version at `/group/halld/Software/builds/Linux_Alma9-x86_64-gcc11.5.0/root/root-6.32.08`

**For other systems**: Install ROOT following the guide at https://root.cern/install/

## Building the Application

### Build Instructions

1. **Configure the build:**

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/JANA2;/path/to/EVIO/Linux-x86_64;/path/to/ROOT"
```

Replace the paths with your actual installation directories:
- `/path/to/JANA2` - Your JANA2 directory
- `/path/to/EVIO` - Your EVIO directory  
- `/path/to/ROOT` - Your ROOT installation (on ifarm set it to `/group/halld/Software/builds/Linux_Alma9-x86_64-gcc11.5.0/root/root-6.32.08`)

2. **Build the application:**

```bash
cmake --build build --parallel
```

**Build Types**: Add `-DCMAKE_BUILD_TYPE=Debug` for debugging or `-DCMAKE_BUILD_TYPE=Release` for optimized builds.

### Complete Build Sequence

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

# 3. Build application (adjust ROOT path as needed)
cd moller_exp
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/JANA2;/path/to/EVIO/Linux-x86_64;/path/to/ROOT"
cmake --build build --parallel
```

**Note**: ROOT installation is not shown here. On ifarm, use the pre-built version. On other systems, install ROOT first (see Dependencies section).

## Usage

After building, run the application with:

```bash
./build/moller [jana_options] <evio_file1> [evio_file2] ...
```

The application will process the specified EVIO files and create a ROOT output file (`moller.root`) containing:
- **Waveform TTree** (`waveform_tree`): Channel-by-channel waveform data with branches for slot, channel, and waveform samples
- **Pulse Integral Histogram** (`h_integral`): Distribution of pulse integral sums

### Customizing Output Filename

You can specify a custom ROOT output filename:

```bash
# Customize ROOT output filename
./build/moller -PROOT_OUT_FILENAME=my_data.root <evio_file>
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