# Security Policy

`terminal` exposes local CLI terminal primitives. It should not read or write
outside the streams passed to it, keep terminal modes changed after failures, or
crash PHP on malformed terminal input.

Security fixes target the latest 1.x release. Upgrade from pre-1.0 releases;
they no longer receive fixes.

| Version | Supported |
| --- | --- |
| 1.x | Yes |
| < 1.0 | No |

Please report security-sensitive issues privately by emailing
prateekbhujelpb@gmail.com. Include the operating system, PHP version, extension
version, terminal environment, and a minimal reproduction if you can share one.

For ordinary bugs, crashes without a security angle, installation problems, or
API feedback, use GitHub issues instead.
