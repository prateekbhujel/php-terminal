# Security Policy

`terminal` exposes local CLI terminal primitives. It should not read or write
outside the streams passed to it, keep terminal modes changed after failures, or
crash PHP on malformed terminal input.

Supported security fixes target the latest released pre-1.0 line.

| Version | Supported |
| --- | --- |
| 0.6.x | Yes |
| < 0.6 | No |

Please report security-sensitive issues privately by emailing
prateekbhujelpb@gmail.com. Include the operating system, PHP version, extension
version, terminal environment, and a minimal reproduction if you can share one.

For ordinary bugs, crashes without a security angle, installation problems, or
API feedback, use GitHub issues instead.
