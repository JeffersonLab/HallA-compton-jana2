# Compton Integration

## Purpose

Run Hall A's faV3 Compton decoder and ROOT processor against the shared JCE EVIO
pipeline, with experiment-owned decoder code and configuration.

## Expected Behavior

- `compton_parser` requests common module parsers, registers `ModuleParser`
  implementation ID 253 through a JANA service, and publishes Hall A hit types.
- Bank routing stays in Compton `config/mapping.db`, with routes for banks 253
  and 595. No duplicate programmatic bank registration.
- JCE does not own or register faV3. Common hit headers come from the installed
  `evio_common_modules_data_types`; Hall A hit headers come from `compton_data_types`.
- `compton_processor` requests its parser and keeps existing ROOT/text output.
- `source setup_compton.sh [build [ROOT_PREFIX]]` and
  `source setup_compton.csh [build [ROOT_PREFIX]]` run from the source directory.
  Build uses the JCE superbuild and installs Compton under `install/`
  (`install/lib/plugins/`, `install/include/`, and `install/config/`).
  `COMPTON_HOME` points to `install/`, which is ignored by Git. ROOT is detected through `root-config --prefix` during the build stage;
  an explicit ROOT prefix overrides the loaded installation.
- Setup preserves existing paths, adds installed configuration and plugins once,
  and shares implementation between shells. Setup alone never builds.
- Build mode clones the default sibling JCE source directory if absent, after validating
  ROOT. Existing source directories are reused without updating them. An explicit
  `JCE_SOURCE_DIR` must select an existing source directory; setup never clones.
- JCE and Compton CMake configurations default to `RelWithDebInfo` when the
  build type is unset or empty; explicit cached choices are preserved.
- ROOT must already be available through `root-config` or an explicit prefix.
  Missing or invalid ROOT installations fail before any CMake commands. Filtering is opt-in and must use validated
  ROC/bank entries; setup does not invent experiment addresses.

## Failure Behavior

- Build/setup failures return a failing status without exiting the caller.
- A stale JCE installation registering ID 253 triggers a duplicate parser error.
- Invalid mapping/filter configuration fails through JCE's loaders.

## Key Components

- `setup_compton.sh`, `setup_compton.csh`, `config/`
- `src/plugins/compton_parser/`, `src/plugins/compton_processor/`

## Verification

- Run `python3 tests/test_compton_setup.py` for both shells, idempotence, and failures.
- Build/install JCE and Compton with real JANA/EVIO/ROOT dependencies.
- Replay a known run and inspect accumulator entries and common raw hits.
