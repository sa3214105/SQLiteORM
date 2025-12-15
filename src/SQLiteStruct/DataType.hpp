#pragma once
namespace TypeSQLite {
    //TODO 分離ExprType以及colType
    enum class DataType {
        TEXT,
        NUMERIC,
        INTEGER,
        REAL,
        BLOB,
        MULTY_TYPE,
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