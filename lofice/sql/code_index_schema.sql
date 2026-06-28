-- lofice codebase index schema (Neon Postgres + pgvector)
-- Apply: neonctl link 후 MCP run_sql 또는 psql

CREATE EXTENSION IF NOT EXISTS vector;

CREATE TABLE IF NOT EXISTS lofice_code_index (
    id BIGSERIAL PRIMARY KEY,
    file_path TEXT NOT NULL,
    chunk_no INT NOT NULL DEFAULT 0,
    language TEXT,
    content TEXT NOT NULL,
    content_sha256 TEXT NOT NULL,
    search_vector tsvector GENERATED ALWAYS AS (
        to_tsvector('simple', coalesce(file_path, '') || ' ' || coalesce(content, ''))
    ) STORED,
    embedding vector(1536),
    indexed_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    UNIQUE (file_path, chunk_no, content_sha256)
);

CREATE INDEX IF NOT EXISTS lofice_code_index_search_idx
    ON lofice_code_index USING gin(search_vector);
CREATE INDEX IF NOT EXISTS lofice_code_index_path_idx
    ON lofice_code_index(file_path);
CREATE INDEX IF NOT EXISTS lofice_code_index_embedding_idx
    ON lofice_code_index USING hnsw (embedding vector_cosine_ops);
