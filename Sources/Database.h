#pragma once
#include <filesystem>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <optional>
#include <string>
#include <mutex>
#include "sqlite_orm.h"
#include "TableModel.h"
#include "Log.h"

template<typename... Tables>
class Database {
public:
    template<typename... Ts>
    static auto make_storage_impl(const std::string& file) {
        return sqlite_orm::make_storage(file, Ts::schema()...);
    }

    using storage_t = decltype(make_storage_impl<Tables...>(""));

    Database() = default;

    explicit Database(const std::string& file) {
        init(file);
    }

    void init(const std::string& file) {
        std::filesystem::path p(file);
        bool exists = std::filesystem::exists(file);

        if (!p.parent_path().empty())
            std::filesystem::create_directories(p.parent_path());

        if (!exists)
            Server::ApplicationLog::Info<Console | File>("Creating database: {}", p.filename().string());
        else
            Server::ApplicationLog::Info<Console | File>("Loading database: {}", p.filename().string());

        storage.emplace(make_storage_impl<Tables...>(file));
        storage->sync_schema();
        load<Tables...>();
    }

    // Get mutable table reference for modifications (insert/update/delete).
    // Returns cached instance if already loaded in this cycle.
    // Reloads only after commit().
    template<typename T>
    T& table() {
        auto it = tables.find(std::type_index(typeid(T)));
        if (it == tables.end()) {
            auto ptr = std::make_unique<T>();
            ptr->template load_from_storage<storage_t>(*storage);
            tables[std::type_index(typeid(T))] = std::move(ptr);
        }
        return *static_cast<T*>(tables.at(std::type_index(typeid(T))).get());
    }

    // Deleted: prevent const reference binding to mutable table
    template<typename T>
    T& table() const = delete;

    // Get const table reference for queries only.
    // Always reloads to ensure fresh data (non-const version).
    template<typename T>
    const T& const_table() {
        auto ptr = std::make_unique<T>();
        ptr->template load_from_storage<storage_t>(*storage);
        tables[std::type_index(typeid(T))] = std::move(ptr);
        return *static_cast<const T*>(tables.find(std::type_index(typeid(T)))->second.get());
    }

    // Get const table reference for queries only (const version).
    // Always reloads to ensure fresh data.
    template<typename T>
    const T& const_table() const {
        auto* non_const_this = const_cast<Database*>(this);
        auto ptr = std::make_unique<T>();
        ptr->template load_from_storage<storage_t>(*const_cast<storage_t*>(non_const_this->storage.operator->()));
        non_const_this->tables[std::type_index(typeid(T))] = std::move(ptr);
        return *static_cast<const T*>(non_const_this->tables.find(std::type_index(typeid(T)))->second.get());
    }

    // Reload table from storage, discarding cache and pending changes
    template<typename T>
    void reload() {
        auto it = tables.find(std::type_index(typeid(T)));
        if (it != tables.end()) {
            auto ptr = std::make_unique<T>();
            ptr->template load_from_storage<storage_t>(*storage);
            tables[std::type_index(typeid(T))] = std::move(ptr);
        }
    }

    void commit() {
        std::scoped_lock lock(db_mutex);

        // Apply all pending operations to storage
        apply_all<Tables...>();

        // Flush transaction
        storage->transaction([&] {
            return true;
            });

        // Clear all pending operations
        clear_all<Tables...>();
    }

private:
    // Load all tables during init
    template<typename First, typename... Rest>
    void load() {
        auto ptr = std::make_unique<First>();
        ptr->template load_from_storage<storage_t>(*storage);
        tables[std::type_index(typeid(First))] = std::move(ptr);
        if constexpr (sizeof...(Rest)) load<Rest...>();
    }

    // Apply pending operations for all tables
    template<typename First, typename... Rest>
    void apply_all() {
        auto it = tables.find(std::type_index(typeid(First)));
        if (it != tables.end()) {
            auto* table_ptr = static_cast<First*>(it->second.get());

            // Copy pending operations before processing
            auto pending_copy = table_ptr->get_pending_copy();

            for (const auto& p : pending_copy) {
                switch (p.op) {
                case Op::Insert:
                    storage->insert(p.row);
                    break;
                case Op::Update:
                    storage->update(p.row);
                    break;
                case Op::Delete:
                    storage->template remove<typename First::row_t>(p.row.id);
                    break;
                }
            }
        }

        if constexpr (sizeof...(Rest)) apply_all<Rest...>();
    }

    // Clear all pending operations for all tables
    template<typename First, typename... Rest>
    void clear_all() {
        auto it = tables.find(std::type_index(typeid(First)));
        if (it != tables.end()) {
            it->second->rollback();
        }

        if constexpr (sizeof...(Rest)) clear_all<Rest...>();
    }

    std::unordered_map<std::type_index, std::unique_ptr<TableModelBase>> tables;
    std::optional<storage_t> storage;
    std::mutex db_mutex;
};