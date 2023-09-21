#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <map>

namespace GTOML::AST {
    enum class NodeType {
        Array,
        String,
        Integer,
        Float,
        Bool,
        Object,
        KeyValue,
    };

    class Node {
        public:
            virtual ~Node() = default;
            virtual NodeType getType() const = 0;
    };

    class StringValue : public Node {
    public:
        StringValue(const std::string& value) : value_(value) {}

        const std::string& getValue() const {
            return value_;
        }

        NodeType getType() const override {
            return NodeType::String;
        }

    private:
        std::string value_;
    };

    class IntValue : public Node {
    public:
        IntValue(int value) : value_(value) {}

        int getValue() const {
            return value_;
        }

        NodeType getType() const override {
            return NodeType::Integer;
        }

    private:
        int value_;
    };

    class FloatValue : public Node {
    public:
        FloatValue(float value) : value_(value) {}

        float getValue() const {
            return value_;
        }

        NodeType getType() const override {
            return NodeType::Float;
        }

    private:
        float value_;
    };

    class BoolValue : public Node {
    public:
        BoolValue(bool value) : value_(value) {}

        bool getValue() const {
            return value_;
        }

        NodeType getType() const override {
            return NodeType::Bool;
        }

    private:
        bool value_;
    };

    class ArrayValue : public Node {
    public:
        ArrayValue(const std::vector<std::shared_ptr<Node>>& elements) : elements_(elements) {}

        const std::vector<std::shared_ptr<Node>>& getElements() const {
            return elements_;
        }

        NodeType getType() const override {
            return NodeType::Array;
        }

        // Method to add a child node to the array.
        void addChild(const std::shared_ptr<Node>& child) {
            elements_.push_back(child);
        }

    private:
        std::vector<std::shared_ptr<Node>> elements_;
    };

    class ObjectValue : public Node {
        public:
            ObjectValue(const std::vector<std::pair<std::string, std::shared_ptr<Node>>>& key_value_pairs)
                : key_value_pairs_(key_value_pairs) {}

            const std::vector<std::pair<std::string, std::shared_ptr<Node>>>& getKeyValuePairs() const {
                return key_value_pairs_;
            }

            void addKeyValuePair(const std::string& key, const std::shared_ptr<Node>& value) {
                key_value_pairs_.emplace_back(key, value);
            }
            NodeType getType() const override {
                return NodeType::Object;
            }

        private:
            std::vector<std::pair<std::string, std::shared_ptr<Node>>> key_value_pairs_;
    };

    class KeyValueNode : public Node {
        public:
            KeyValueNode(const std::string& key, const std::shared_ptr<Node>& value)
                : key_(key), value_(value) {}

            const std::string& getKey() const {
                return key_;
            }

            const std::shared_ptr<Node>& getValue() const {
                return value_;
            }

            NodeType getType() const override {
                return NodeType::KeyValue;
            }

        private:
            std::string key_;
            std::shared_ptr<Node> value_;
    };

    using Value = std::variant<std::shared_ptr<StringValue>,
          std::shared_ptr<IntValue>, std::shared_ptr<FloatValue>,
          std::shared_ptr<BoolValue>, std::shared_ptr<ArrayValue>,
          std::shared_ptr<ObjectValue>>;

} // namespace GTOML::AST

