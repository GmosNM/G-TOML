#include <iostream>

#include "parser.hpp"

using namespace GTOML;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }
  try {
    Parser parser(argv[1]);

    std::string version = parser.getValueByKey("version");
    std::cout << version << std::endl;
    std::string use_small_heuristics =
        parser.getValueByKey("use_small_heuristics");
    std::cout << use_small_heuristics << std::endl;
    std::string merge_derives = parser.getValueByKey("merge_derives");
    std::cout << merge_derives << std::endl;

    Node arr = parser.getNodeByKey("ignore");

    std::cout << "\n\nIGNORE ARRAY: \n\n" << std::endl;
    std::vector<std::string> array = parser.ArrayToVector(arr);
    for (auto element : array) {
      std::cout << element << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
