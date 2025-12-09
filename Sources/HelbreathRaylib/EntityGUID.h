#pragma once
#include <cstdint>
#include <atomic>
#include <functional>

// Custom GUID implementation to avoid conflicts with Windows GUID
// Uses a simple incrementing 64-bit counter for uniqueness
class EntityGUID {
public:
    EntityGUID() : _id(generate_id()) {}

    // Allow explicit construction from uint64_t for special cases
    explicit EntityGUID(uint64_t id) : _id(id) {}

    uint64_t get_id() const { return _id; }

    bool operator==(const EntityGUID& other) const {
        return _id == other._id;
    }

    bool operator!=(const EntityGUID& other) const {
        return _id != other._id;
    }

    bool operator<(const EntityGUID& other) const {
        return _id < other._id;
    }

    // Check if this is a valid GUID (non-zero)
    bool is_valid() const {
        return _id != 0;
    }

    // Get an invalid GUID
    static EntityGUID invalid() {
        return EntityGUID(0);
    }

private:
    uint64_t _id;

    static uint64_t generate_id() {
        static std::atomic<uint64_t> counter{1}; // Start from 1, 0 is invalid
        return counter.fetch_add(1, std::memory_order_relaxed);
    }
};

// Hash function for EntityGUID to use in unordered containers
namespace std {
    template<>
    struct hash<EntityGUID> {
        size_t operator()(const EntityGUID& guid) const noexcept {
            return hash<uint64_t>()(guid.get_id());
        }
    };
}
