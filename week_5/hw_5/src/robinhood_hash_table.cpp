#include "robinhood_hash_table.hpp"
#include <cmath>
#include <stdexcept>

template<typename Key, typename Value>
RobinHoodHashTable<Key, Value>::RobinHoodHashTable(size_t initial_capacity)
    : table(initial_capacity), num_elements(0), capacity(initial_capacity) {}

template<typename Key, typename Value>
size_t RobinHoodHashTable<Key, Value>::hash(const Key& key) const {
    return std::hash<Key>{}(key);
}

template<typename Key, typename Value>
bool RobinHoodHashTable<Key, Value>::insert(const Key& key, const Value& value) {
    if ((float)num_elements / capacity > max_load_factor) rehash();

    size_t idx = hash(key) % capacity;
    size_t probe_dist = 0;
    Entry entry{key, value, true, 0};

    while (table[idx].occupied) {
        if (table[idx].key == key) {
            table[idx].value = value;
            return false; // Already present, updated
        }
        if (table[idx].probe_distance < probe_dist) {
            std::swap(entry, table[idx]);
        }
        idx = (idx + 1) % capacity;
        ++probe_dist;
        entry.probe_distance = probe_dist;
    }
    table[idx] = entry;
    ++num_elements;
    return true;
}

template<typename Key, typename Value>
bool RobinHoodHashTable<Key, Value>::erase(const Key& key) {
    size_t idx = hash(key) % capacity;
    size_t probe_dist = 0;
    while (table[idx].occupied) {
        if (table[idx].key == key) {
            table[idx].occupied = false;
            --num_elements;
            return true;
        }
        idx = (idx + 1) % capacity;
        ++probe_dist;
        if (probe_dist > capacity) break;
    }
    return false;
}

template<typename Key, typename Value>
std::optional<Value> RobinHoodHashTable<Key, Value>::find(const Key& key) const {
    size_t idx = hash(key) % capacity;
    size_t probe_dist = 0;
    while (table[idx].occupied) {
        if (table[idx].key == key) {
            return table[idx].value;
        }
        idx = (idx + 1) % capacity;
        ++probe_dist;
        if (probe_dist > capacity) break;
    }
    return std::nullopt;
}

template<typename Key, typename Value>
void RobinHoodHashTable<Key, Value>::clear() {
    table.clear();
    table.resize(capacity);
    num_elements = 0;
}

template<typename Key, typename Value>
void RobinHoodHashTable<Key, Value>::rehash() {
    size_t new_capacity = capacity * 2;
    std::vector<Entry> old_table = std::move(table);
    table = std::vector<Entry>(new_capacity);
    capacity = new_capacity;
    num_elements = 0;
    for (const auto& entry : old_table) {
        if (entry.occupied) {
            insert(entry.key, entry.value);
        }
    }
}


template class RobinHoodHashTable<std::string, double>;