# HallA-compton-jana2
## Set up
1. Firstly, install "jana2-common-extensions (https://github.com/JeffersonLab/jana2-common-extensions)"
2. $JCE_HOME should have been set at this point
3. Source "sourceme.sh"; this sets up $JCE_CONFIG_DIR to [config](config)
4. Then run "${JCE_HOME}/scripts/jce.sh /path/to/data.evio"
## Useful user flags
All parameters are set on the JANA2 command line with `-P<name>=<value>`.

| Parameter | Default | `is_shared` | Description |
|---|---|---|---|
| `ROOT_OUT_FILENAME` | `evio_processor.root` | yes | Path/name of the ROOT output file |
| `TXT_OUT_FILENAME` | `evio_processor_hits.txt` | yes | Path/name of the text hit-summary file |
| `jana:nevents` |  | | Number of events to process |
