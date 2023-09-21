#include <vector>
#include <string>
#include <memory>
#include <map>


class TOMLNode{
public:
  virtual ~TOMLNode(){};
};

// AST node for key-value pairs
class KeyValueNode : public TOMLNode {
public:
    KeyValueNode(const std::string& key, std::shared_ptr<TOMLNode> value)
        : key(key), value(value) {}

    std::string key;
    std::shared_ptr<TOMLNode> value;
};

// AST node for string values
class StringNode : public TOMLNode {
public:
    StringNode(const std::string& value) : value(value) {}

    std::string value;
};

// AST node for integer values
class IntegerNode : public TOMLNode {
public:
    IntegerNode(int value) : value(value) {}

    int value;
};

// AST node for TOML tables
class TableNode : public TOMLNode {
public:
    TableNode(const std::vector<std::shared_ptr<KeyValueNode>>& entries)
        : entries(entries) {}

    std::vector<std::shared_ptr<KeyValueNode>> entries;
};


// AST node for boolean values
class BoolNode : public TOMLNode {
public:
    BoolNode(bool value) : value(value) {}

    bool value;
};

// AST node for float values
class FloatNode : public TOMLNode {
public:
    FloatNode(float value) : value(value) {}

    float value;
};


class ArrayNode : public TOMLNode {
public:
    ArrayNode(const std::vector<std::shared_ptr<TOMLNode>>& elements)
        : elements(elements) {}

    std::vector<std::shared_ptr<TOMLNode>> elements;
};