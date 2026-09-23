# Documentation

The documentation is kept in the repository and versioned with release tags. For
a released version, open these files from that tag to get documentation matching
the code you installed.

## Start here

- [README](../README.md) for installation and a first session
- [API guide](api.md) for the stable `Io\Terminal` surface and error model
- [Compatibility](compatibility.md) for PHP, platform and stream boundaries
- [Integration guide](integration.md) for optional framework or library adoption
- [Examples](../examples) for runnable diagnostics, streams and prompt loops

## Project maintenance

- [Contributing](../CONTRIBUTING.md) for bug reports, development and PHPT expectations
- [Security policy](../SECURITY.md) for private vulnerability reports
- [Release process](releasing.md) for maintainer release steps
- [Changelog](../CHANGELOG.md) for user-visible changes

The canonical public signatures are also declared in
[`terminal.stub.php`](../terminal.stub.php). The stub is useful when checking an
exact method signature; the guides describe behavior and platform constraints.

## Why no documentation website?

The project is still small enough that versioned Markdown is easier to keep
correct than a second generated documentation site. GitHub Pages can be added
later if the guides grow substantially or if versioned API documentation needs
search/navigation beyond what the repository provides.
