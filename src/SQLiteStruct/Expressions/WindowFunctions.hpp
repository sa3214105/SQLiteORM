#pragma once
#include <cstddef>
#include <string>
#include <tuple>

#include "../DataType.hpp"
#include "../Column/Column.hpp"
#include "../Expressions/Expressions.hpp"

namespace TypeSQLite {
    template<DataType exprResultType, typename WindowFuncCols, typename WindowFuncParams, typename PartitionBy, typename
        OrderExpr>
    struct WindowFuncInfo {
        constexpr static DataType resultType = exprResultType;
        const std::string windowFuncSql;
        const WindowFuncCols windowFuncCols;
        const WindowFuncParams windowFuncParams;
        PartitionBy partitionBy;
        OrderExpr orderExpr;
        OrderType orderType = OrderType::ASC;
    };

    template<DataType exprResultType, typename WindowFuncCols, typename WindowFuncParams, typename PartitionBy, typename
        OrderExpr>
    auto MakeInfo(std::string sql, WindowFuncCols cols, WindowFuncParams params, PartitionBy partitionBy,
                  OrderExpr orderExpr, OrderType orderType = OrderType::ASC) {
        return WindowFuncInfo<exprResultType, WindowFuncCols, WindowFuncParams, PartitionBy, OrderExpr>{
            .windowFuncSql = sql,
            .windowFuncCols = cols,
            .windowFuncParams = params,
            .partitionBy = partitionBy,
            .orderExpr = orderExpr,
            .orderType = orderType
        };
    }

    template<typename NewInfo>
    std::string CreateSQLPartitionBy(const NewInfo &_info) {
        if constexpr (std::is_same_v<decltype(_info.partitionBy), nullptr_t>) {
            return "";
        } else {
            return std::apply([](auto... exprs) {
                return " PARTITION BY " + GetExprSqls(exprs...);
            }, _info.partitionBy);
        }
    }

    template<typename NewInfo>
    std::string CreateSQLOrderBy(const NewInfo &_info) {
        if constexpr (std::is_same_v<decltype(_info.orderExpr), nullptr_t>) {
            return "";
        } else {
            return " ORDER BY " + _info.orderExpr.sql + " " + OrderTypeToString(_info.orderType);
        }
    }

    template<typename NewInfo>
    auto GetSql(const NewInfo &_info) {
        return _info.windowFuncSql + " OVER(" +
               CreateSQLPartitionBy(_info) +
               CreateSQLOrderBy(_info) +
               ")";
    }

    template<typename NewInfo>
    auto GetInfoCols(const NewInfo &_info) {
        return std::tuple_cat(_info.windowFuncCols, GetExprsTupleColTuple(_info.partitionBy),
                              GetExprsColTuple(_info.orderExpr));
    }

    template<typename NewInfo>
    auto GetInfoParams(const NewInfo &_info) {
        return std::tuple_cat(_info.windowFuncParams, GetExprsTupleParamTuple(_info.partitionBy),
                              GetExprsParamTuple(_info.orderExpr));
    }

    //TODO frame_clause 未實作
    template<typename Columns, typename Parameters, typename Info>
    class WindowFunctions {
    public:
        constexpr static DataType resultType = Info::resultType;
        const Info info;
        const Columns cols;
        const Parameters params;
        const std::string sql;

        template<ExprOrColConcept... Exprs>
        auto PartitionedBy(Exprs... exprs) {
            auto _info = MakeInfo<resultType>(
                info.windowFuncSql,
                info.windowFuncCols,
                info.windowFuncParams,
                std::make_tuple(exprs...),
                info.orderExpr,
                info.orderType
            );
            auto newCols = GetInfoCols(_info);
            auto newParams = GetInfoParams(_info);
            return WindowFunctions<decltype(newCols),decltype(newParams),decltype(_info)>{
                .info = _info,
                .cols = newCols,
                .params = newParams,
                .sql = GetSql(_info)
            };
        }

        template<ExprOrColConcept Expr>
        auto OrderBy(Expr expr, const OrderType order = OrderType::ASC) {
            auto _info = MakeInfo<resultType>(
                info.windowFuncSql,
                info.windowFuncCols,
                info.windowFuncParams,
                info.partitionBy,
                expr,
                order
            );
            auto newCols = GetInfoCols(_info);
            auto newParams = GetInfoParams(_info);
            return WindowFunctions<decltype(newCols),decltype(newParams),decltype(_info)>{
                .info = _info,
                .cols = newCols,
                .params = newParams,
                .sql = GetSql(_info)
            };
        }
    };

    template<typename>
    struct IsWindowFunctions : std::false_type {
    };

    template<typename Columns, typename Parameters, typename Info>
    struct IsWindowFunctions<WindowFunctions<Columns, Parameters, Info> > : std::true_type {
    };

    template<typename T>
    concept WindowFunctionsConcept = IsWindowFunctions<T>::value;

    template<DataType DataType, ExprOrColConcept ... Exprs>
    auto MakeWindowFunction(std::string newSQL, Exprs... exprs) {
        auto newCols = std::tuple_cat(GetCols(exprs)...);
        auto newPara = std::tuple_cat(GetParms(exprs)...);
        auto info = MakeInfo<DataType>(
            newSQL,
            newCols,
            newPara,
            nullptr,
            nullptr
        );
        return WindowFunctions{
            .info = info,
            .cols = GetInfoCols(info),
            .params = GetInfoParams(info),
            .sql = GetSql(info)
        };
    }

    inline auto RowNumber() {
        return MakeWindowFunction<DataType::INTEGER>(" ROW_NUMBER()");
    }

    inline auto Rank() {
        return MakeWindowFunction<DataType::INTEGER>(" RANK()");
    }

    inline auto DenseRank() {
        return MakeWindowFunction<DataType::INTEGER>(" DENSE_RANK()");
    }

    inline auto PercentRank() {
        return MakeWindowFunction<DataType::NUMERIC>(" PERCENT_RANK()");
    }

    inline auto CumeDist() {
        return MakeWindowFunction<DataType::NUMERIC>(" CUME_DIST()");
    }

    //TODO 限定int Expr
    template<ExprOrColConcept Expr>
    auto NTile(Expr n) {
        return MakeWindowFunction<DataType::INTEGER>(" NTILE(" + n.sql + ")", n);
    }

    template<ExprOrColConcept Expr>
    auto Lag(Expr expr) {
        return MakeWindowFunction<Expr::resultType>(" LAG(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept Expr, ExprOrColConcept Offset>
    auto Lag(Expr expr, Offset offset) {
        return MakeWindowFunction<Expr::resultType>(" LAG(" + expr.sql + ", " + offset.sql + ")", expr, offset);
    }

    template<ExprOrColConcept Expr, ExprOrColConcept Offset, ExprOrColConcept Default>
    auto Lag(Expr expr, Offset offset, Default defaultValue) {
        return MakeWindowFunction<Expr::resultType>(
            " LAG(" + expr.sql + ", " + offset.sql + ", " + defaultValue.sql + ")", expr, offset,
            defaultValue);
    }

    template<ExprOrColConcept Expr>
    auto Lead(Expr expr) {
        return MakeWindowFunction<Expr::resultType>(" LEAD(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept Expr, ExprOrColConcept Offset>
    auto Lead(Expr expr, Offset offset) {
        return MakeWindowFunction<Expr::resultType>(" LEAD(" + expr.sql + ", " + offset.sql + ")", expr, offset);
    }

    template<ExprOrColConcept Expr, ExprOrColConcept Offset, ExprOrColConcept Default>
    auto Lead(Expr expr, Offset offset, Default defaultValue) {
        return MakeWindowFunction<Expr::resultType>(
            " LEAD(" + expr.sql + ", " + offset.sql + ", " + defaultValue.sql + ")", expr, offset,
            defaultValue);
    }

    template<ExprOrColConcept Expr>
    auto FirstValue(Expr expr) {
        return MakeWindowFunction<Expr::resultType>(" FIRST_VALUE(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept Expr>
    auto LastValue(Expr expr) {
        return MakeWindowFunction<Expr::resultType>(" LAST_VALUE(" + expr.sql + ")", expr);
    }

    template<ExprOrColConcept Expr, ExprOrColConcept N>
    auto NthValue(Expr expr, N n) {
        return MakeWindowFunction<Expr::resultType>(" NTH_VALUE(" + expr.sql + ", " + n.sql + ")", expr, n);
    }
}
