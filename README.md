# modconv

A C++ command-line tool for converting and editing proprietary 3D model files (`.mod` format) from Pikmin 1 for the GameCube and Wii.

## Table of Contents

- [Features](#features)
- [Building and Running](#building-and-running)
- [Usage](#usage)
- [System Design](#system-design)
- [Future Enhancements](#future-enhancements)

## Features

- **File Handling**
  - Load, parse, and write `.mod` files
  - Delete specific data chunks (materials, textures, vertices) from loaded models

- **Import / Export**
  - Export / import material and TEV (Texture Environment) settings to human-readable text files
  - Export model geometry to Wavefront `.obj` format
  - Export all textures as individual `.txe` files
  - Import data from previously exported `.txe` and material files
  - Import and export trailing `.ini` data blocks
  - Export model data to `.dmd` format [WIP]

## Building and Running

### Prerequisites

- Modern C++ compiler (GCC, Clang, or MSVC)

- CMake

### Compilation

Navigate to the project root directory:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Running

Execute the compiled binary to start the interactive shell:

```bash
./modconv[.exe]
```

## Usage

The tool uses an interactive command-line interface. Once started, it displays available commands.

### Interactive Mode

**Basic workflow:**

1. Load a `.mod` file
2. Export data to editable formats or modify the file as needed
3. Apply changes to the model
4. Write the modified model to a new file

**Example session:**

```bash
load my_model.mod
export_materials materials.txt
# Edit materials.txt with a text editor
import_material materials.txt
write my_model_new.mod
close
```

### Command-Line

### 1. Convert MOD to OBJ

```bash
# Windows
modconv.exe load model.mod export_obj model.obj

# Linux/Mac
./modconv load model.mod export_obj model.obj
```

### 2. Extract all textures from a MOD file

```bash
modconv load model.mod export_textures ./textures/
```

### 3. Export and reimport materials

```bash
modconv load model.mod export_materials materials.txt close load model2.mod import_material materials.txt write model2_updated.mod
```

### 4. Modify and save a MOD file

```bash
# Delete collision data and save
modconv load model.mod delete_chunk 0x100 delete_chunk 0x110 write model_no_collision.mod
```

## System Design

- **`MOD` Class**: Central class representing a loaded `.mod` file, containing vectors and objects for all data chunks (vertices, materials, meshes)

- **`cmd` Namespace**: Manages the interactive command-line interface, parsing user input and executing corresponding functions

- **File I/O**
  - **`util::fstream_reader` & `util::fstream_writer`**: Custom stream classes for binary file I/O with big-endian byte swapping
  - **`MaterialReader` & `MaterialWriter`**: Classes for serializing material data to/from human-readable text format

- **Data Structures**: There are a variety of `struct`s that directly map to binary structures in the `.mod` file format (`Material`, `TEVInfo`, `Mesh`, `Joint`, `CollTriInfo`, etc.)

## Future Enhancements

- Complete work-in-progress `.obj` and `.dmd` export functionality
- Implement the stubbed `objToDmd` converter
- Add support for importing geometry from standard formats like `.obj`
- Tidy the code further
