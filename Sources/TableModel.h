#pragma once
#include <vector>
#include <algorithm>
#include "Log.h"

enum class Op { Insert, Update, Delete };

template<typename T>
struct PendingOp {
    Op op;
    T row;
};

struct TableModelBase {
    virtual ~TableModelBase() = default;
    virtual void stage() = 0;
    virtual void rollback() = 0;
};

template<typename RowT>
class TableModel : public TableModelBase {
public:
    using row_t = RowT;

    const std::vector<RowT>& data() const { return cache; }

    void insert(const RowT& r) {
        cache.push_back(r);
        local.push_back({ Op::Insert, r });
    }

    void update(const RowT& r) {
        auto it = std::find_if(cache.begin(), cache.end(),
            [&](auto& x) { return x.id == r.id; });
        if (it != cache.end()) {
            *it = r;
            local.push_back({ Op::Update, r });
        }
    }

    void remove(int id) {
        auto it = std::find_if(cache.begin(), cache.end(),
            [&](auto& x) { return x.id == id; });
        if (it != cache.end()) {
            local.push_back({ Op::Delete, *it });
            cache.erase(it);
        }
    }

    template<typename Storage>
    void load_from_storage(Storage& s) {
        cache = s.template get_all<RowT>();
        original = cache;
        local.clear();
        db_pending.clear();
    }

    void stage() override {
        for (auto& op : local) {
            db_pending.push_back(op);
        }
        local.clear();
    }

    void rollback() override {
        db_pending.clear();
        local.clear();
    }

    // Get a copy of pending operations for safe iteration
    std::vector<PendingOp<RowT>> get_pending_copy() const {
        return db_pending;
    }

    size_t pending_count() const {
        return db_pending.size();
    }

private:
    std::vector<RowT> cache;
    std::vector<RowT> original;
    std::vector<PendingOp<RowT>> local;
    std::vector<PendingOp<RowT>> db_pending;
};