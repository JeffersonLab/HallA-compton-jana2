# FADC250 Data Processing Application

A JANA2-based application for compton experiment to process data from EVIO-format files.

## Data Processing Flow

The application uses a two-level event structure (block-level and physics event-level) to process EVIO data:

1. **JEventSource_EVIO** reads EVIO files and creates block-level events:
   - Extracts run numbers from run control events (tags 0xFFD0-0xFFDF) and skips them
   - Wraps physics events in `EvioEventWrapper` objects and inserts them into block-level events

2. **JFactory_PhysicsEvent** processes block-level events:
   - Extracts `EvioEventWrapper` objects from the event
   - Uses `EvioEventParser` together with `BankParser` implementations registered in `JEventService_BankParsersMap` to decode each EVIO bank
   - Creates `PhysicsEvent` objects containing detector hits (waveforms and pulses)

3. **JEventUnfolder_EVIO** unfolds block-level events into physics event-level child events:
   - Takes `PhysicsEvent` objects from the block-level parent event
   - Creates individual physics event-level child events

4. **JEventProcessor_Compton** processes physics event-level events:
   - Receives FADC250 waveform and pulse hits
   - Writes waveform data to a ROOT TTree
   - Fills histograms with pulse integral distributions
   - Outputs results to a ROOT file

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

The application will process the specified EVIO files and create a ROOT output file (`compton.root`).
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

### Data Objects (`parser/data_objects/` and `user_parsers/FADC/data_objects/`)
- `parser/data_objects/PhysicsEvent.h` – Container for physics event data including event number and hits
- `parser/data_objects/EventHits.h` – Abstract container interface for all event hits
- `parser/data_objects/EvioEventWrapper.h` – JANA2 object wrapper for EVIO events (allows `std::shared_ptr` to be passed through JANA2 pipeline)
- `user_parsers/FADC/data_objects/FADC250Hit.h` – Base class for all FADC250 hits
- `user_parsers/FADC/data_objects/FADC250PulseHit.h` – Pulse hit data structure
- `user_parsers/FADC/data_objects/FADC250WaveformHit.h` – Waveform hit data structure
- `user_parsers/FADC/data_objects/EventHits_FADC.h` – FADC-specific implementation of `EventHits` that knows how to insert hits into a `JEvent`

### Parser Utilities (`parser/`)
- `parser/EvioEventParser.cc/.h` – EVIO event parsing utilities
- `parser/BankParser.h` – Base interface for all bank parsers (hardware-agnostic)
- `JEventService_BankParsersMap.h` – JANA service mapping bank IDs to `BankParser` implementations

### User Parsers (`user_parsers/`)

User-defined hardware parsers live under `user_parsers/`. The provided example for FADC250 is in `user_parsers/FADC/`.

- **FADC250 example**
  - `user_parsers/FADC/BankParser_FADC.cc/.h` – `BankParser` implementation for FADC250 data
  - `user_parsers/FADC/data_objects/` – FADC-specific hit and event container classes

## Adding a new bank parser (step by step)

1. **Pick a bank ID and hardware type**
   - Decide which EVIO bank tag you want to handle (e.g. `350` for some new module).
   - Confirm the data format for that bank (word layout, headers, trailers, etc.).

2. **Create a new user parser directory**
   - Under `user_parsers/`, create a new subdirectory for your hardware, e.g.:
     - `user_parsers/MyHW/`
     - `user_parsers/MyHW/data_objects/`
   - Use `user_parsers/FADC/` as a reference for directory layout.

3. **Define hardware-specific hit and event containers**
   - In `user_parsers/MyHW/data_objects/`, create:
     - Hit classes (similar to `FADC250Hit`, `FADC250PulseHit`, `FADC250WaveformHit`) that capture the decoded fields you care about.
     - A subclass of `EventHits` (similar to `EventHits_FADC`) which:
       - Owns `std::vector<...*>` of your hit types.
       - Implements `void insertIntoEvent(JEvent& event) override` to `event.Insert(...)` those hits.

4. **Implement a new `BankParser` subclass**
   - In `user_parsers/MyHW/BankParser_MyHW.h`:
     - Include `BankParser.h` and your hit / `EventHits` headers.
     - Declare a class `BankParser_MyHW : public BankParser` and override:
       - `void parse(std::shared_ptr<evio::BaseStructure> data_block, uint32_t rocid, std::vector<PhysicsEvent*>& physics_events, uint64_t block_first_event_num) override;`
   - In `user_parsers/MyHW/BankParser_MyHW.cc`:
     - Implement `parse(...)` by:
       - Extracting 32-bit words from the EVIO bank (`getUIntData()`).
       - Decoding headers/trailers/words for your hardware.
       - Filling a `std::shared_ptr<EventHits_MyHW>` as you walk the words.
       - Creating `PhysicsEvent*` objects with `std::shared_ptr<EventHits>` (upcast from `EventHits_MyHW`) and pushing them into `physics_events`.
     - Reuse `BankParser::getBitsInRange(...)` helper to extract bit fields, similar to `BankParser_FADC`.

5. **Expose your new parser to CMake**
   - Edit `CMakeLists.txt`:
     - Add your implementation file to the `SOURCES` list, for example:
       - `user_parsers/MyHW/BankParser_MyHW.cc`
     - Add include directories if needed:
       - `${CMAKE_CURRENT_SOURCE_DIR}/user_parsers/MyHW`
       - `${CMAKE_CURRENT_SOURCE_DIR}/user_parsers/MyHW/data_objects`

6. **Register the parser with the service**
   - In `compton.cc`, after creating the application and service:
     - Get the service:
       - `auto bank_parsers_service = app.GetService<JEventService_BankParsersMap>();`
     - Register your parser for the chosen bank ID:
       - `bank_parsers_service->addParser(<bank_id>, new BankParser_MyHW());`
   - The `EvioEventParser` will automatically call `JEventService_BankParsersMap::getParser(bank_id)` for each EVIO bank and dispatch decoding to your parser when the bank tag matches.

7. **Rebuild and test**
   - Re-run CMake and build:
     - `cmake -S . -B build` (if you changed CMake)
     - `cmake --build build --parallel`
   - Run `./build/compton ...` on EVIO files containing your new bank and verify:
     - `PhysicsEvent` objects now contain your new hits.
     - `JEventUnfolder_EVIO` passes them to downstream processors (e.g. `JEventProcessor_Compton` or your own processor).
