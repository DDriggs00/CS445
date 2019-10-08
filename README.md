# VGo Compiler Project

This is a toy compiler made for CS445, the Compiler Design Class at University of Idaho

## Prerequisites

The latest version of the following must be installed, and in a PATH directory

- make
- gcc
- Flex
- Bison

## Installation

1. Navigate to the folder containing the makefile
2. Run `make`
3. Copy `vgo`, the generated file, to a folder in the PATH system variable

## Usage

To compile any number of VGo-compatible Go source files, simply run the vgo executable, with the files as arguments. Example Below.

```sh
vgo ./main.go ./other.go
```

## Warning to Students

To future students of CS445 at University of Idaho, this project is well known by Dr. J knows of this repository, and will heavily punish any plagiarism. This is designed only to be used as a reference, to be used to figure out how certain parts of the project work (especially flex/bison).

For more information on how this can and cannot be used, ask Dr. J.

[A copy of the University of Idaho Academic Integrity policy can be found here](https://www.uidaho.edu/student-affairs/dean-of-students/student-conduct/academic-integrity)
