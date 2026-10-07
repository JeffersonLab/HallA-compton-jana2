# HallA-compton-jana2

## Build and Setup

Use `RelWithDebInfo` for optimized processing with debug symbols for profiling.
`Release` is also suitable for optimized runs. An empty CMake build type is not
equivalent to an optimized build. The JCE superbuild forwards the selected type
to JANA2, EVIO, and JCE; rebuild and install after changing it.
CMake defaults to `RelWithDebInfo` when no build type is selected; an explicit
build type in the CMake cache is preserved.

Build mode clones `jana2-common-extensions` beside this repository if it is
missing, or reuses the existing checkout. Set `JCE_SOURCE_DIR` to use another
existing checkout; an empty or invalid override fails without cloning.
Setup mode never clones. JCE must include configuration layering and
must no longer register the Hall A faV3 decoder. ROOT must already be installed. Every build invocation requires its installation
prefix explicitly; omitting it fails before any build commands run.

From the Compton checkout, in tcsh:

```tcsh
source halla.csh build /path/to/ROOT
"${JCE_HOME}/scripts/jce.csh" /path/to/data.evio
```

Or in Bash:

```bash
source halla.sh build /path/to/ROOT
"${JCE_HOME}/scripts/jce.sh" /path/to/data.evio
```

The build step builds JCE's pinned JANA/EVIO stack, then builds and installs
Compton directly into the current checkout, matching Hall C: plugins under
`lib/plugins/`, headers under `include/`, and configuration under `config/`. It does not download or install ROOT.
The first stack build may download dependencies. An optional `JCE_HOME` selects
another stack installation prefix. ROOT search paths in `CMAKE_PREFIX_PATH`
are retained. Build commands stop at the first failure without closing the
calling shell.

On ifarm, pass the installed ROOT prefix during the build stage (replace the
placeholder with your site's ROOT installation directory):

```tcsh
source halla.csh build /path/on/ifarm/to/ROOT
```

```bash
source halla.sh build /path/on/ifarm/to/ROOT
```

The required argument selects `ROOT_DIR` from that installation's
`ROOTConfig.cmake`, overriding any cached ROOT selection. Invalid prefixes fail
before the build begins. It is not
needed when sourcing setup later; the build keeps its configuration in `build/`.

After installation, `source halla.csh` or `source halla.sh` sets up an existing
installation without building. Run from the Compton checkout. Setup selects
`JCE_HOME`, sets `COMPTON_HOME` to the checkout directory, prepends Compton's plugin directory,
and appends its installed config directory to `JCE_CONFIG_DIR`. Existing path
entries are preserved, and repeated sourcing does not duplicate entries.
The tcsh script sources environment settings produced by the Bash implementation;
executing a child shell alone cannot set the caller's environment.
These scripts replace `sourceme.sh`.

## EVIO Replay

Core default plugins include `evio_parser`, `evio_common_modules`, and
`detector_translation`. Compton's `default_plugins.db` adds `compton_parser`
and `compton_processor`. The parser registers decoder ID 253 once. Compton owns
bank routes `253 -> 253` and `595 -> 253` (file format: module then bank).
Loading `compton_processor` explicitly also requests `compton_parser`, which
requests the common module parsers. ROOT output is `evio_processor.root` and
text output is `evio_processor_hits.txt` by default, in the current directory.
Existing ROOT output is overwritten.

```bash
"${JCE_HOME}/scripts/jce.sh" -Pjana:nevents=10 /path/to/data.evio
```

Bank mapping and filter files merge core first, then directories listed in
`JCE_CONFIG_DIR`. Explicit `BANKMAP:FILE` and `FILTER:FILE` replace only core;
directory additions still apply. Filtering is disabled by default. Before
using `FILTER:ENABLE=1`, validate `config/filter.db` against the actual run:
it currently has bank `3654` while the helicity mapping is `3564`, and it has
no bank `595` entry. Add the actual ROC/bank combinations needed for your data;
the setup does not guess them.

Hall A's decoder and raw-hit headers live in this repository. Rebuild JCE after
updating both checkouts so an older common plugin cannot register decoder 253
alongside Compton. Translation catalogs retain their existing experiment
registration; this change does not add Compton detector translators.

## Verification

Run `python3 tests/test_halla_setup.py` for shell setup and build orchestration.
It uses temporary installations and a mock CMake command, without downloading
or building dependencies. For physics validation, build against actual JCE/ROOT,
replay a known EVIO sample, and check nonzero accumulator entries in
`compton_tree` together with expected raw-hit counts.

## Useful user flags
All parameters are set on the JANA2 command line with `-P<name>=<value>`.

| Parameter | Default | `is_shared` | Description |
|---|---|---|---|
| `ROOT_OUT_FILENAME` | `evio_processor.root` | yes | Path/name of the ROOT output file |
| `TXT_OUT_FILENAME` | `evio_processor_hits.txt` | yes | Path/name of the text hit-summary file |
| `jana:nevents` |  | | Number of events to process |
