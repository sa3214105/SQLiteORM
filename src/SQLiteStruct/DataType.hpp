#pragma once
namespace TypeSQLite {
    enum class DataType {
        TEXT,
        NUMERIC,
        INTEGER,
        REAL,
        BLOB
    };
    template<DataType type>
    constexpr auto DataTypeToString() {
        switch (type) {
            case DataType::TEXT:
                return "TEXT";
            case DataType::NUMERIC:
                return "NUMERIC";
            case DataType::INTEGER:
                return "INTEGER";
            case DataType::REAL:
                return "REAL";
            case DataType::BLOB:
                return "BLOB";
            default:
                return "UNKNOWN";
        }
    }
}