#pragma once
#include "DB/Field.h"
#include <vector>

class QueryResult
{
public:
    void AddRow(std::vector<Field>&& r) { rows.push_back(std::move(r)); }

    bool NextRow()
    {
        if (index + 1 >= rows.size()) return false;
        ++index;
        return true;
    }

    const Field& operator[](size_t i) const { return rows[index][i]; }
    bool Empty() const { return rows.empty(); }

    size_t RowCount() const { return rows.size(); }

private:
    std::vector<std::vector<Field>> rows;
    size_t index = 0;
};