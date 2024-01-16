#include "include/huffman.h"

#include <stdexcept>

constexpr size_t kSize = 16;

class HuffmanTree::Impl {
public:
    Impl() {
        root_ = std::make_shared<Node>();
        current_ = root_;
    }

    ~Impl() {
        if (root_) {
            InOrder(root_);
        }
    }

    void Build(const std::vector<uint8_t> &code_lengths, const std::vector<uint8_t> &values) {
        if (root_) {
            InOrder(root_);
        }
        root_->left = nullptr;
        root_->right = nullptr;
        current_ = root_;
        if (code_lengths.size() > kSize) {
            throw std::invalid_argument("Huffman too big");
        }
        std::vector<uint8_t> codes = code_lengths;
        size_t i = 0;
        size_t depth = 0;
        size_t index_of_values = 0;
        int order = 1;
        while (i < codes.size()) {
            if (codes[i] != 0) {
                if (depth == i + 1) {
                    if (index_of_values >= values.size()) {
                        throw std::invalid_argument("Huffman short");
                    }
                    current_->value = values[index_of_values];
                    current_->used = true;
                    current_ = root_;
                    depth = 0;
                    ++order;
                    ++index_of_values;
                    --codes[i];
                } else {
                    if (!current_->left) {
                        std::shared_ptr<Node> left = std::make_shared<Node>();
                        current_->left = left;
                        left->parent = current_;
                        current_ = left;
                        ++depth;
                    } else if (!current_->left->used && current_->left->visited != order) {
                        current_ = current_->left;
                        ++depth;
                    } else if (!current_->right) {
                        std::shared_ptr<Node> right = std::make_shared<Node>();
                        current_->right = right;
                        right->parent = current_;
                        current_ = right;
                        ++depth;
                    } else if (!current_->right->used && current_->right->visited != order) {
                        current_ = current_->right;
                        ++depth;
                    } else {
                        if (current_->parent) {
                            current_->visited = order;
                            current_ = current_->parent;
                            --depth;
                        } else {
                            throw std::invalid_argument("Can't build");
                        }
                    }
                }
            } else {
                ++i;
            }
        }
    }

    bool Move(bool bit, int &value) {
        if (bit) {
            if (current_->right) {
                if (current_->right->right || current_->right->left) {
                    current_ = current_->right;
                } else {
                    value = current_->right->value;
                    current_ = root_;
                    return true;
                }
            } else {
                throw std::invalid_argument("Can't move");
            }
        } else {
            if (current_->left) {
                if (current_->left->left || current_->left->right) {
                    current_ = current_->left;
                } else {
                    value = current_->left->value;
                    current_ = root_;
                    return true;
                }
            } else {
                throw std::invalid_argument("Can't move");
            }
        }
        return false;
    }

private:
    struct Node {
        std::shared_ptr<Node> right;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> parent;
        uint8_t value;
        int visited;
        bool used;
        Node() {
            right = nullptr;
            left = nullptr;
            visited = 0;
            used = false;
        }
    };
    std::shared_ptr<Node> root_;
    std::shared_ptr<Node> current_;

    void InOrder(std::shared_ptr<Node> node) {
        if (node->left) {
            InOrder(node->left);
        }
        node->parent = nullptr;
        if (node->right) {
            InOrder(node->right);
        }
    }
};

HuffmanTree::HuffmanTree() {
    impl_ = std::make_unique<Impl>();
};

void HuffmanTree::Build(const std::vector<uint8_t> &code_lengths,
                        const std::vector<uint8_t> &values) {
    impl_->Build(code_lengths, values);
}

bool HuffmanTree::Move(bool bit, int &value) {
    return impl_->Move(bit, value);
}

HuffmanTree::HuffmanTree(HuffmanTree &&) = default;

HuffmanTree &HuffmanTree::operator=(HuffmanTree &&) = default;

HuffmanTree::~HuffmanTree() = default;
