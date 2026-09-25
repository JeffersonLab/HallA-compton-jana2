# HallA-compton-jana2

## Prerequisites

Install [JANA2](https://github.com/JeffersonLab/JANA2),
[EVIO](https://github.com/JeffersonLab/evio),
[jana2-common-extensions](https://github.com/JeffersonLab/jana2-common-extensions),
and ROOT before building Compton. Build and install JCE against the same JANA2
and EVIO installations. Its CMake package finds JANA2 and EVIO automatically,
but CMake still needs their installation prefixes to locate them. ROOT must
also be discoverable for `compton_processor`.

## Build And Install

From a `csh` shell, replace the example paths with your installation prefixes.
`JANA_HOME` must contain `bin/jana`; `JCE_HOME` must contain `scripts/jce.csh`;
the EVIO and ROOT prefixes must contain their CMake package files.

```csh
setenv COMPTON_HOME /path/to/HallA-compton-jana2
setenv JANA_HOME /path/to/JANA2/install
setenv EVIO_PREFIX /path/to/evio/install
setenv JCE_HOME /path/to/jana2-common-extensions/install
setenv ROOT_PREFIX /path/to/ROOT/install
cd "$COMPTON_HOME"
cmake -S . -B build -DCMAKE_PREFIX_PATH="$JANA_HOME;$EVIO_PREFIX;$JCE_HOME;$ROOT_PREFIX" -DCMAKE_INSTALL_PREFIX="$COMPTON_HOME/install"
cmake --build build --parallel
cmake --install build
```

## Replay EVIO Data

In the same shell, use Compton's installed plugins and this repository's
configuration. Run from the Compton repository root:

```csh
setenv JANA_PLUGIN_PATH "$COMPTON_HOME/install/lib/plugins"
setenv JCE_CONFIG_DIR "$COMPTON_HOME/config"
cd "$COMPTON_HOME"
$JCE_HOME/scripts/jce.csh data/comptonFADC_485.evio.0
```

Replace the sample path with another EVIO file to replay a different run. Add
`-Pjana:nevents=10` to limit the source to ten EVIO records. The processor writes
`evio_processor.root` and `evio_processor_hits.txt` in the working directory.
An existing ROOT output file at that path is overwritten.

## Useful user flags
All parameters are set on the JANA2 command line with `-P<name>=<value>`.

| Parameter | Default | `is_shared` | Description |
|---|---|---|---|
| `ROOT_OUT_FILENAME` | `evio_processor.root` | yes | Path/name of the ROOT output file |
| `TXT_OUT_FILENAME` | `evio_processor_hits.txt` | yes | Path/name of the text hit-summary file |
| `jana:nevents` |  | | Number of events to process |
