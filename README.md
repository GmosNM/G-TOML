# G-TOML

G-TOML is a C++ library for parsing TOML (Tom's Obvious, Minimal Language) files. It allows you to read and manipulate TOML configuration files in your C++ applications.

## Features

- **TOML Parsing**: G-TOML provides a robust parser for TOML files, allowing you to read and manipulate TOML data in your C++ applications.

- **Simple API**: The library offers an easy-to-use API for interacting with TOML data, making it straightforward to read and modify configuration files.

- **Data Types**: G-TOML supports various data types, including strings, integers, floating-point numbers, booleans, arrays, and tables.

## Getting Started

Follow these instructions to get started with G-TOML in your C++ project.

### Prerequisites

- C++17 or higher.
- CMake (for building).

### Installation

1. Clone the G-TOML repository to your local machine.

   ```shell
   git clone https://github.com/GmosNM/G-TOML.git
   ```

2. Build the library using CMake.

   ```shell
   cd G-TOML
   cmake -GNinja -B bld
   cmake --build bld
   ```

3. Link the G-TOML library to your C++ project.

### Usage

Here's a basic example of how to use G-TOML to parse a TOML file:

```cpp
#include <iostream>
#include "gtoml/parser.hpp"

int main() {
    GTOML::Parser parser("config.toml");
    if (parser.parse()) {
        // Access and manipulate TOML data here
    } else {
        std::cerr << "Failed to parse TOML file." << std::endl;
    }

    return 0;
}
```

For more detailed usage and examples, please refer to the [documentation](https://github.com/GmosNM/G-TOML/wiki).

## Contributing

Contributions are welcome! If you'd like to contribute to G-TOML, please follow our [contribution guidelines](CONTRIBUTING.md).

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- G-TOML is inspired by the [TOML](https://toml.io) specification.

## Contact

For any questions or issues, please open a GitHub issue on the [project repository](https://github.com/GmosNM/G-TOML/issues).
