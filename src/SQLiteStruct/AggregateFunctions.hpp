#pragma once
#include "../TemplateHelper/FixedString.hpp"
#include "Column.hpp"

namespace SQLiteHelper {
    template<ColumnOrTableColumnConcept Col>
    struct Avg : Column<"AVG", column_type::REAL> {
        constexpr static FixedString name = FixedString("AVG(") + GetColumnName<Col>() + FixedString(")");
    };

    template<ColumnOrTableColumnConcept Col>
    struct Count : Column<"COUNT", column_type::INTEGER> {
        constexpr static FixedString name = FixedString("COUNT(") + GetColumnName<Col>() + FixedString(")");
        constexpr static column_type type = column_type::INTEGER;
        using ValueType = int;
        ValueType value;
    };

    template<ColumnOrTableColumnConcept Col1, ColumnOrTableColumnConcept Col2>
    struct GroupConcat : Column<"GROUP_CONCAT", column_type::TEXT> {
        constexpr static FixedString name = FixedString("GROUP_CONCAT(") + GetColumnName<Col1>() + FixedString(", ")
                                            + GetColumnName<Col2>() + FixedString(")");
        constexpr static column_type type = column_type::TEXT;
        using ValueType = std::string;
        ValueType value;
    };

    template<ColumnOrTableColumnConcept Col>
    struct Max : Column<"MAX", column_type::REAL> {
        constexpr static FixedString name = FixedString("MAX(") + GetColumnName<Col>() + FixedString(")");
    };

    template<ColumnOrTableColumnConcept Col>
    struct Median : Column<"MAX", column_type::REAL> {
        constexpr static FixedString name = FixedString("MEDIAN(") + GetColumnName<Col>() + FixedString(")");
    };

    template<ColumnOrTableColumnConcept Col>
    struct Min : Column<"MAX", column_type::REAL> {
        constexpr static FixedString name = FixedString("MIN(") + GetColumnName<Col>() + FixedString(")");
    };

    template<ColumnOrTableColumnConcept Col, double percent>
    struct Percentile: Column<"PERCENT", column_type::REAL> {
        constexpr static FixedString name = FixedString("PERCENTILE(") + GetColumnName<Col>() + FixedString(", ") +
                                            toFixedString<percent> + FixedString(")");
    };

    template<ColumnOrTableColumnConcept Col, double percent>
    struct PercentileCont: Column<"PERCENT", column_type::REAL> {
        constexpr static FixedString name = FixedString("PERCENTILE_CONT(") + GetColumnName<Col>() + FixedString(", ") +
                                            toFixedString<percent> + FixedString(")");
    };

    template<ColumnOrTableColumnConcept Col>
    struct Sum: Column<"PERCENT", column_type::REAL> {
        constexpr static FixedString name = FixedString("SUM(") + GetColumnName<Col>() + FixedString(")");
    };

    template<ColumnOrTableColumnConcept Col>
    struct Total: Column<"PERCENT", column_type::REAL> {
        constexpr static FixedString name = FixedString("TOTAL(") + GetColumnName<Col>() + FixedString(")");
    };

    template<typename>
    struct IsAggregateFunction : std::false_type {
    };

    template<ColumnOrTableColumnConcept Column>
    struct IsAggregateFunction<Avg<Column> > : std::true_type {
    };

    template<ColumnOrTableColumnConcept Column>
    struct IsAggregateFunction<Count<Column> > : std::true_type {
    };

    template<ColumnOrTableColumnConcept Column1, ColumnOrTableColumnConcept Column2>
    struct IsAggregateFunction<GroupConcat<Column1, Column2> > : std::true_type {
    };

    template<ColumnOrTableColumnConcept Column>
    struct IsAggregateFunction<Max<Column> > : std::true_type {
    };

    template<ColumnOrTableColumnConcept Column>
    struct IsAggregateFunction<Min<Column> > : std::true_type {
    };

    template<ColumnOrTableColumnConcept Column>
    struct IsAggregateFunction<Median<Column> > : std::true_type {
    };

    template<ColumnOrTableColumnConcept Column, double percent>
    struct IsAggregateFunction<Percentile<Column, percent> > : std::true_type {
    };

    template<ColumnOrTableColumnConcept Column, double percent>
    struct IsAggregateFunction<PercentileCont<Column, percent> > : std::true_type {
    };

    template<ColumnOrTableColumnConcept Column>
    struct IsAggregateFunction<Sum<Column> > : std::true_type {
    };

    template<ColumnOrTableColumnConcept Column>
    struct IsAggregateFunction<Total<Column> > : std::true_type {
    };

    template<typename T>
    concept AggregateFunctionConcept = IsAggregateFunction<T>::value;
}
