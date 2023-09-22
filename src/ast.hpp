#include <vector>
#include <string>
#include <memory>
#include <map>

class TOMLNode {
public:
    virtual ~TOMLNode() {};

    bool inside_table = false;
    std::string table_name;

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
    TableNode(const std::vector<std::shared_ptr<TOMLNode>>& entries)
        : entries(entries) {}

    std::vector<std::shared_ptr<TOMLNode>> entries;
    std::string name;
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
    FloatNode(const std::string& value) : value(value) {}

    std::string value;
};

// AST node for array
class ArrayNode : public TOMLNode {
public:
    ArrayNode(const std::string& name, const std::vector<std::shared_ptr<TOMLNode>>& elements)
        : array_name(name), elements(elements) {}

    const std::string array_name;
    std::vector<std::shared_ptr<TOMLNode>> elements;
};

