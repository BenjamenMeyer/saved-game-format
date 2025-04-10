Overview
========
Simple C++ project to demonstrate to use a compressed file format
similar to JAR and ODF files.

Goals
=====
- Demonstrate an application that can load a zip file in-memory
- Dump the contents of files in the zip file
- Add a new file to the zip file
- Update an existing file in the zip file

Requirements
============
- Use the Boost C++ Library
  - Boost File System
  - Boost IOStream
  - Boost JSON
  - Boost Program Options
- CMake
- Command-line interface that specifies:
  - the zip file to operate on
  - commands:
    - update
    - add
    - dump
  - options
    - file inside the zip file to operate on
    - key-value to add to or update in the file
    - key to read from the file
    - new filename to write to
