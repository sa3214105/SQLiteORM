#pragma once
namespace SQLiteHelper {
    enum class ConflictCause {
        ROLLBACK,
        ABORT,
        FAIL,
        IGNORE,
        REPLACE
    };

    template<ConflictCause Cause>
    constexpr auto ConflictCauseToString() {
        if constexpr (Cause == ConflictCause::ROLLBACK) {
            return FixedString(" ON CONFLICT ROLLBACK");
        } else if constexpr (Cause == ConflictCause::ABORT) {
            return FixedString(" ON CONFLICT ABORT");
        } else if constexpr (Cause == ConflictCause::FAIL) {
            return FixedString(" ON CONFLICT FAIL");
        } else if constexpr (Cause == ConflictCause::IGNORE) {
            return FixedString(" ON CONFLICT IGNORE");
        } else if constexpr (Cause == ConflictCause::REPLACE) {
            return FixedString(" ON CONFLICT REPLACE");
        }
    }
}
