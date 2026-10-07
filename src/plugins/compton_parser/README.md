# compton_parser

Hall A owns the faV3 decoder and raw-hit types. The plugin requests
`evio_common_modules` and provides a service which registers parser ID 253.
The processor consumes the published hits using `compton_data_types`.

Use the Hall C `module_parser_example` as a guide for service registration;
Compton bank routes remain in `config/mapping.db` so file overlays can change
them. Do not also register those banks with `addRoute()`.

See [integration behavior](../../../docs/compton-integration.md) and
[setup/replay instructions](../../../README.md).
