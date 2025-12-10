# FADC250 Data Processing Application

A JANA2-based application for processing FADC250 detector data from EVIO-format files. For now, the application supports extracting and decoding FADC250 data only.

## Data Processing Flow

The application uses a two-level event structure (block-level and physics event-level) to process EVIO data:

1. **JEventSource_EVIO** reads EVIO files and creates block-level events:
   - Extracts run numbers from run control events (tags 0xFFD0-0xFFDF) and skips them
   - Wraps physics events in `EvioEventWrapper` objects and inserts them into block-level events

2. **JFactory_PhysicsEvent** processes block-level events:
   - Extracts `EvioEventWrapper` objects from the event
   - Uses `EvioEventParser` to parse the raw EVIO event structure
   - Creates `PhysicsEvent` objects containing detector hits (waveforms and pulses)

3. **JEventUnfolder_EVIO** unfolds block-level events into physics event-level child events:
   - Takes `PhysicsEvent` objects from the block-level parent event
   - Creates individual physics event-level child events

4. **JEventProcessor_Compton** processes physics event-level events:
   - Receives FADC250 waveform and pulse hits
   - Writes waveform data to a ROOT TTree
   - Fills histograms with pulse integral distributions
   - Outputs results to a ROOT file

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
- ROOT 6.36.04 (for data analysis and visualization)

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

Install ROOT following the guide at https://root.cern/install/

## Building the Application

### Build Instructions

1. **Configure the build:**

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/JANA2;/path/to/EVIO/Linux-x86_64;/path/to/ROOT"
```

Replace the paths with your actual installation directories:
- `/path/to/JANA2` - Your JANA2 directory
- `/path/to/EVIO` - Your EVIO directory  
- `/path/to/ROOT` - Your ROOT installation

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
cd compton_exp
cmake -S . -B build -DCMAKE_PREFIX_PATH="/path/to/JANA2;/path/to/EVIO/Linux-x86_64;/path/to/ROOT"
cmake --build build --parallel
```

**Note**: ROOT installation is not shown here, install ROOT first (see Dependencies section).

## Usage

After building, run the application with:

```bash
./build/compton [jana_options] <evio_file1> [evio_file2] ...
```

The application will process the specified EVIO files and create a ROOT output file (`compton.root`) containing:
- **Waveform TTree** (`waveform_tree`): Channel-by-channel waveform data with branches for slot, channel, and waveform samples
- **Pulse Integral Histogram** (`h_integral`): Distribution of pulse integral sums

### Customizing Output Filename

You can specify a custom ROOT output filename:

```bash
# Customize ROOT output filename
./build/compton -PROOT_OUT_FILENAME=my_data.root <evio_file>
```

## Project Structure

### Core Application Files
- `compton.cc` – Main application entry point that registers all components
- `JEventSource_EVIO.cc/.h` – EVIO file event source (block-level events)
- `JFactory_PhysicsEvent.cc/.h` – Factory that creates PhysicsEvent objects from EVIO events
- `JEventUnfolder_EVIO.h` – Unfolder that creates physics event-level child events from block-level events
- `JEventProcessor_Compton.cc/.h` – Main event processor that writes data to ROOT file

### Data Objects (`data_objects/`)
- `PhysicsEvent.h` – Container for physics event data including event number and hits
- `EventHits.h` – Container for all event hits
- `EvioEventWrapper.h` – JANA2 object wrapper for EVIO events (allows shared_ptr to be passed through JANA2 pipeline)
- `FADC250Hit.h` – Base class for all FADC250 hits
- `FADC250PulseHit.h` – Pulse hit data structure
- `FADC250WaveformHit.h` – Waveform hit data structure

### Parser Utilities (`parsers/`)
- `EvioEventParser.cc/.h` – EVIO event parsing utilities
- `RawDataParser.cc/.h` – Raw data parsing utilities
