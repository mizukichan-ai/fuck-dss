# Fuck .DS-Store

A simple C99 utility to clean up macOS-specific files when transferring files off Macs.

## Features

- Removes macOS-specific files like `.DS_Store`, `._*`, `.AppleDouble`, `.DS_Store?`
- Removes Spotlight-related files like `.Spotlight-V100`, `.fseventsd`, `.metadata_never_index`
- Removes trash files like `.Trashes`, `.Trash`
- Recursively crawls directories (can be disabled)
- Configurable flags to ignore specific file types
- Verbose output option

## Build

```bash
make
```

## Usage

```bash
fuck-dss -<flags> <targetdir>
```

### Flags

- `v`: Report program activity to stdout
- `n`: Don't crawl (only process top-level directory)
- `d`: Ignore DSS files
- `s`: Ignore Spotlight files
- `t`: Ignore trash files

### Examples

```bash
# Remove all macOS files recursively with verbose output
fuck-dss -v /path/to/directory

# Remove all files except DSS files
fuck-dss -d /path/to/directory

# Only process top-level directory, no recursion
fuck-dss -n /path/to/directory

# Remove only Spotlight files
fuck-dss -s /path/to/directory
```

## Target

Any POSIX-compatible OS with standard C99 libraries.

## License

None. Do whatever you want with it.