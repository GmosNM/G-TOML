#include <iostream>
#include "../src/parser.hpp"


using namespace GTOML;

int main() {
    Parser toml("tests/test.toml");
    if (toml.Parse()) {
        toml.printIR();
        auto package_name = toml.getTableValue("package.name");
        std::cout << package_name << std::endl;

        auto package_version = toml.getTableValue("package.version");
        std::cout << package_version << std::endl;

        auto is_experimental = toml.getTableValue("package.is_experimental");
        std::cout << is_experimental << std::endl;


    } else {
        std::cerr << "Failed to parse TOML file." << std::endl;
    }

    return 0;
}
