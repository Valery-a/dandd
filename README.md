## Setup Instructions

### Prerequisites

Ensure you have the following installed on your system:

- **CMake** (minimum version recommended: 3.10)
- **A C++ compiler** (GCC, Clang, or MSVC)
- **Make or Ninja** (depending on your platform)

### Build Instructions

1. **Clone the Repository**:

   ```sh
   git clone https://github.com/Valery-a/dandd
   cd dandd-main
   ```

2. **Create a Build Directory and Enter it**:

   ```sh
   mkdir -p build
   cd build
   ```

3. **Run CMake to Generate Build Files:**

   ```sh
   cmake ..
   ```

   This will configure the project and generate the necessary build system files.

4. **Compile the Project:**

   ```sh
   cmake --build .
   ```

   This step compiles the source code and produces the executable.

### Running the Executable

After a successful build, the compiled executable will be located in the `build/` directory. Run it using:

```sh
./run
```

## Cleaning Up

To remove all generated files, you can run:

```sh
rm -rf build
```

This will delete the `build/` directory, allowing you to start fresh if needed.

## Troubleshooting

- Ensure CMake is installed and available in your system's PATH.
- Verify that a compatible C++ compiler is installed.
- If using Windows, consider using the CMake GUI or running the commands in a developer command prompt.

