# Change Log
All notable changes to `Project-Template` will be documented here.

## [1.1.0] - Unreleased

### To be added
- Automatic text-wrapping in descriptions.
- Extra attributes for variables such as `defaultValue` and `required`.
- Recursive variable initialization. (E.g: `sentence="My name is !name!"`, `name="Jimmy"`)
- Multi-line values for variables.
- Proper indentation for multi-line variable values.

## [1.0.0] - 2024/09/11

First release version.

## [1.0.0-beta.3] - 2024/09/10

### Added
- Added update option with `-U,--update`.
- `config reset` option to bring back config to default settings.
- `config reset <template>` option to bring back template config to default settings.

### Fixed
- Fixed issue with cache not updating.

## [1.0.0-beta.2] - 2024/08/29

### Added
- Cache for search paths.

### Fixed
- Fixed pattern matching

## [1.0.0-beta.1] - 2024/08/14

### Added
- Parameterized template initialization.