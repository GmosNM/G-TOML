#include <iostream>

#include "parser.hpp"

using namespace GTOML;

int main() {
    GTOML::Parser parser("test.toml");
    if (parser.Parse()) {
        std::string version = parser.getValueByKey("version");
        std::cout << version << std::endl;
        std::string use_small_heuristics = parser.getValueByKey("use_small_heuristics");
        std::cout << use_small_heuristics << std::endl;
        std::string merge_derives = parser.getValueByKey("merge_derives");
        std::cout << merge_derives << std::endl;

        Node array = parser.getNodeByKey("ignore");
        std::vector<std::string> vec = parser.ArrayToVector(array);
        for (std::string s : vec) {
            std::cout << s << std::endl;
        }
    } else {
        std::cerr << "Failed to parse TOML file." << std::endl;
    }

    return 0;
}
