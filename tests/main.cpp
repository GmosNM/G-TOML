#include <iostream>
#include "../src/parser.hpp"


using namespace GTOML;

int main() {
    Parser parser("tests/test.toml");
    if (parser.Parse()) {
        auto package_name = parser.getTableValue("package.name");
        std::cout << "name: " << package_name << std::endl;
        auto package_version = parser.getTableValue("package.version");
        std::cout << "version: " << package_version << std::endl;

        std::cout << "Array: " << std::endl;
        auto array = parser.getArrayValue("array");
        for (auto value : array) {
            std::cout << value << std::endl;
        }

    } else {
        std::cerr << "Failed to parse TOML file." << std::endl;
    }

    return 0;
}
