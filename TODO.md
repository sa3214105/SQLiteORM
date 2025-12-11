# TODO List

## Missing SQLite Features to Implement

### 1. View Support (視圖支援)
- [ ] CREATE VIEW
- [ ] DROP VIEW
- [ ] Query from views

### 2. Trigger Support (觸發器支援)
- [ ] CREATE TRIGGER
- [ ] DROP TRIGGER
- [ ] BEFORE/AFTER/INSTEAD OF triggers
- [ ] INSERT/UPDATE/DELETE trigger events

### 3. ALTER TABLE Support (表結構修改)
- [ ] ALTER TABLE RENAME TO
- [ ] ALTER TABLE ADD COLUMN
- [ ] ALTER TABLE DROP COLUMN
- [ ] ALTER TABLE RENAME COLUMN

### 4. PRAGMA Statements (資料庫設定)
- [ ] PRAGMA foreign_keys
- [ ] PRAGMA journal_mode
- [ ] PRAGMA synchronous
- [ ] PRAGMA cache_size
- [ ] PRAGMA temp_store
- [ ] PRAGMA user_version
- [ ] PRAGMA table_info

### 5. Common Table Expressions (CTE) (公用資料表運算式)
- [ ] WITH clause (non-recursive)
- [ ] WITH RECURSIVE clause
- [ ] Multiple CTEs in single query

### 6. CASE Expression (條件運算式)
- [ ] CASE WHEN ... THEN ... ELSE ... END
- [ ] Simple CASE expression
- [ ] Searched CASE expression

### 7. Subquery Support (子查詢)
- [ ] Scalar subqueries
- [ ] IN subquery
- [ ] EXISTS subquery
- [ ] Correlated subqueries

### 8. CAST and Type Conversion (型別轉換)
- [ ] CAST(expr AS type)
- [ ] Type affinity functions

### 9. Date and Time Functions (日期時間函數)
- [ ] datetime()
- [ ] date()
- [ ] time()
- [ ] strftime()
- [ ] julianday()
- [ ] Date arithmetic

### 10. String Functions (字串函數)
- [ ] SUBSTR/SUBSTRING
- [ ] CONCAT (|| operator)
- [ ] TRIM/LTRIM/RTRIM
- [ ] UPPER/LOWER
- [ ] LENGTH
- [ ] REPLACE
- [ ] LIKE/GLOB pattern matching
- [ ] INSTR
- [ ] printf()

### 11. Math Functions (數學函數)
- [ ] ABS
- [ ] ROUND
- [ ] CEIL/CEILING
- [ ] FLOOR
- [ ] RANDOM
- [ ] POWER
- [ ] SQRT
- [ ] MOD

### 12. NULL Handling (NULL 處理)
- [ ] COALESCE
- [ ] IFNULL
- [ ] NULLIF
- [ ] IS NULL / IS NOT NULL operators

### 13. GROUP BY Enhancements (分組增強功能)
- [ ] HAVING clause
- [ ] GROUP_CONCAT function
- [ ] GROUPING SETS (if supported by SQLite version)

### 14. ORDER BY Enhancements (排序增強功能)
- [ ] NULLS FIRST / NULLS LAST
- [ ] COLLATE clause
- [ ] Multiple sort keys

### 15. UNION Operations (聯集操作)
- [ ] UNION
- [ ] UNION ALL
- [ ] INTERSECT
- [ ] EXCEPT

### 16. Prepared Statement Optimization (預處理語句優化)
- [ ] Statement caching
- [ ] Batch execution optimization

### 17. ATTACH/DETACH Database (附加/分離資料庫)
- [ ] ATTACH DATABASE
- [ ] DETACH DATABASE
- [ ] Cross-database queries

### 18. Backup and Restore (備份與還原)
- [ ] Database backup API
- [ ] Export to SQL
- [ ] Import from SQL

### 19. User-Defined Functions (使用者自訂函數)
- [ ] Register custom scalar functions
- [ ] Register custom aggregate functions
- [ ] Register custom collation sequences

### 20. JSON Support (JSON 支援)
- [ ] JSON1 extension functions
- [ ] json()
- [ ] json_extract()
- [ ] json_array()
- [ ] json_object()

### 21. Full-Text Search (全文搜尋)
- [ ] FTS5 table creation
- [ ] MATCH operator
- [ ] FTS5 auxiliary functions
- [ ] Ranking and snippets

### 22. Conflict Resolution (衝突解決)
- [ ] ON CONFLICT ROLLBACK
- [ ] ON CONFLICT ABORT
- [ ] ON CONFLICT FAIL
- [ ] ON CONFLICT IGNORE
- [ ] ON CONFLICT REPLACE

### 23. VACUUM and Database Maintenance (資料庫維護)
- [ ] VACUUM
- [ ] VACUUM INTO
- [ ] ANALYZE
- [ ] REINDEX

### 24. Savepoint Support (儲存點支援)
- [ ] SAVEPOINT
- [ ] RELEASE SAVEPOINT
- [ ] ROLLBACK TO SAVEPOINT

### 25. Schema Introspection (結構描述檢查)
- [ ] List all tables
- [ ] Get table schema
- [ ] List all indexes
- [ ] Get index info

### 26. EXPLAIN Query Plan (查詢計畫說明)
- [ ] EXPLAIN
- [ ] EXPLAIN QUERY PLAN

### 27. Blob Operations (二進位大型物件操作)
- [ ] Incremental Blob I/O
- [ ] Blob binding and retrieval

### 28. WHERE Clause Enhancements (WHERE 子句增強)
- [ ] IN operator with list
- [ ] BETWEEN operator
- [ ] GLOB operator
- [ ] REGEXP operator (if enabled)

### 29. Safety Features (安全功能)
- [x] Force WHERE clause for UPDATE (需要 WHERE 才能執行)
- [x] Force WHERE clause for DELETE (需要 WHERE 才能執行)
- [ ] Query timeout
- [ ] Result row limit
- [ ] Dry-run mode

### 30. Performance Optimization (效能優化)
- [ ] Connection pooling
- [ ] Lazy loading
- [ ] Query result streaming
- [ ] Memory-mapped I/O

---

## Recently Completed Features
- ✅ Basic CRUD operations (Insert/Select/Update/Delete)
- ✅ Transaction support with automatic commit/rollback
- ✅ Join operations (INNER/LEFT/RIGHT/FULL/CROSS)
- ✅ Window functions
- ✅ Aggregate functions (COUNT/SUM/AVG/MIN/MAX)
- ✅ Column constraints (PRIMARY KEY/NOT NULL/UNIQUE/DEFAULT/CHECK)
- ✅ Table constraints (FOREIGN KEY)
- ✅ Index support
- ✅ DISTINCT queries
- ✅ LIMIT/OFFSET pagination
- ✅ Expression support (comparison operators, arithmetic)
- ✅ Batch insert operations
- ✅ UPSERT (INSERT OR REPLACE)
- ✅ Table options (WITHOUT ROWID/STRICT)
- ✅ Force WHERE for UPDATE/DELETE safety

---

## Priority (優先順序)

### High Priority (高優先)
1. Subquery support (needed for complex queries)
2. CASE expressions (common in business logic)
3. String functions (LIKE, CONCAT, etc.)
4. GROUP BY HAVING
5. UNION operations

### Medium Priority (中優先)
6. Date/Time functions
7. NULL handling functions (COALESCE, IFNULL)
8. ALTER TABLE support
9. View support
10. Math functions

### Low Priority (低優先)
11. Trigger support
12. Full-text search
13. JSON support
14. User-defined functions
15. ATTACH/DETACH

### Nice to Have (可選功能)
16. Backup/Restore utilities
17. Schema introspection tools
18. Query plan explanation
19. Connection pooling
20. Advanced optimizations

---

**Last Updated:** 2025-12-12

