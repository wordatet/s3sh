# s3sh - AT&T System III Shell Port to 64-bit Linux

This repository contains a functional port of the original AT&T System III Bourne Shell (`sh`) to modern 64-bit Linux.

## About the Port
This project focuses on preserving the historical integrity of the System III shell while ensuring stability on modern architectures. Key modifications include:
- **Memory Management**: Refactored to use standard `malloc`/`free` and a `realloc`-expandable stack, replacing the legacy `sbrk`-based custom allocator.
- **64-bit Stability**: Resolved numerous pointer truncation issues and 64-bit compatibility bugs.
- **Functional Stability**: Verified with a comprehensive stress test suite covering variables, pipes, loops, and signal handling.

## Attribution & Collaboration
This port was achieved through a collaborative human-AI effort between **Mario (@wordatet)** and AI assistants from **Google DeepMind (Gemini)** and **Anthropic (Claude)**.

The AI assistants were instrumental in:
- Identifying subtle pointer truncation issues in legacy C code.
- Refactoring the core memory model for 64-bit safety.
- Debugging NULL pointer dereferences that were technically "legal" on 1980s hardware but crash modern systems.

## Documentation & Compliance
For a detailed technical overview of the changes made, see the architectural notes and porting analysis in the source files.

**Important Legal Notice**: Users of this repository are responsible for complying with the historical licensing terms of the original AT&T source code. Helpful resources for understanding the "Ancient UNIX" licensing context are provided below for educational guidance only and do not constitute legal advice:
- [The Unix Heritage Society (TUHS)](https://www.tuhs.org/)
- [Caldera License for Ancient UNIX](https://www.tuhs.org/Archive/Caldera-license.pdf)

## License
The original AT&T System III source code remains subject to its original licensing terms. **Modifications and porting shims provided in this repository are licensed under the S3SH Preservationist License.** See `LICENSE` for details.
