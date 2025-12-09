#pragma once
#include "EntityGUID.h"
#include "entities.h"
#include <memory>
#include <unordered_map>
#include <vector>
#include <type_traits>

class EntityManager {
public:
    EntityManager() = default;
    ~EntityManager() = default;

    // Disable copy and move semantics
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
    EntityManager(EntityManager&&) = delete;
    EntityManager& operator=(EntityManager&&) = delete;

    // Add entity using template (emplace-like behavior)
    // Returns a reference to the newly created entity
    template<typename T, typename... Args>
    T& Add(Args&&... args) {
        static_assert(std::is_base_of<Entity, T>::value, "T must derive from Entity");

        // Create the entity with forwarded arguments
        auto entity_ptr = std::make_unique<T>(std::forward<Args>(args)...);

        // Get the GUID and a reference before moving the pointer
        EntityGUID guid = entity_ptr->get_guid();
        T& entity_ref = *entity_ptr;

        // Store in both containers
        _entities.push_back(std::move(entity_ptr));
        _entity_map[guid] = _entities.back().get();

        return entity_ref;
    }

    // Get entity by GUID
    Entity* Get(const EntityGUID& guid) {
        auto it = _entity_map.find(guid);
        if (it != _entity_map.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Get entity by GUID with type checking
    template<typename T>
    T* Get(const EntityGUID& guid) {
        static_assert(std::is_base_of<Entity, T>::value, "T must derive from Entity");

        Entity* entity = Get(guid);
        if (entity) {
            return dynamic_cast<T*>(entity);
        }
        return nullptr;
    }

    // Remove entity by GUID
    bool Remove(const EntityGUID& guid) {
        auto it = _entity_map.find(guid);
        if (it == _entity_map.end()) {
            return false;
        }

        // Find and remove from vector
        auto vec_it = std::find_if(_entities.begin(), _entities.end(),
            [&guid](const std::unique_ptr<Entity>& entity) {
                return entity->get_guid() == guid;
            });

        if (vec_it != _entities.end()) {
            _entities.erase(vec_it);
        }

        // Remove from map
        _entity_map.erase(it);
        return true;
    }

    // Clear all entities
    void Clear() {
        _entities.clear();
        _entity_map.clear();
    }

    // Get count of entities
    size_t Count() const {
        return _entities.size();
    }

    // Check if entity exists
    bool Exists(const EntityGUID& guid) const {
        return _entity_map.find(guid) != _entity_map.end();
    }

    // Iterate over all entities
    void ForEach(std::function<void(Entity&)> func) {
        for (auto& entity : _entities) {
            func(*entity);
        }
    }

    // Iterate over all entities with type filtering
    template<typename T>
    void ForEach(std::function<void(T&)> func) {
        static_assert(std::is_base_of<Entity, T>::value, "T must derive from Entity");

        for (auto& entity : _entities) {
            T* typed_entity = dynamic_cast<T*>(entity.get());
            if (typed_entity) {
                func(*typed_entity);
            }
        }
    }

    // Get all entities as a vector reference (read-only access)
    const std::vector<std::unique_ptr<Entity>>& GetAll() const {
        return _entities;
    }

    // Iterator support for range-based for loops
    // Allows: for (auto& entity : entity_manager) { ... }
    auto begin() { return _entities.begin(); }
    auto end() { return _entities.end(); }
    auto begin() const { return _entities.begin(); }
    auto end() const { return _entities.end(); }

    // Const iterators
    auto cbegin() const { return _entities.cbegin(); }
    auto cend() const { return _entities.cend(); }

private:
    std::vector<std::unique_ptr<Entity>> _entities;
    std::unordered_map<EntityGUID, Entity*> _entity_map;
};
