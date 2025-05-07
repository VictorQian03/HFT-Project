#pragma once
#include <string>
#include <vector>
#include <optional>
#include <functional>

template<typename Key, typename Value>
class RobinHoodHashTable {
public:
    RobinHoodHashTable(size_t initial_capacity = 1024);

    bool insert(const Key& key, const Value& value);
    bool erase(const Key& key);
    std::optional<Value> find(const Key& key) const;
    void clear();
    size_t size() const { return num_elements; }

private:
    struct Entry {
        Key key;
        Value value;
        bool occupied = false;
        size_t probe_distance = 0;
    };

    std::vector<Entry> table;
    size_t num_elements;
    size_t capacity;
    float max_load_factor = 0.85f;

    size_t hash(const Key& key) const;
    void rehash();
};