/**
 * lofice — shared RAG search (Neon FTS / semantic)
 */
import { neon } from '@neondatabase/serverless';
import { embedQuery, vectorLiteral } from './openai-embeddings.mjs';
import { buildContextFromRows } from './rag-context.mjs';

async function searchFullText(sql, query, limit) {
  return sql`
    SELECT file_path, chunk_no, language, content,
           left(content, 600) AS preview,
           ts_rank(search_vector, websearch_to_tsquery('simple', ${query})) AS rank
    FROM lofice_code_index
    WHERE search_vector @@ websearch_to_tsquery('simple', ${query})
    ORDER BY rank DESC
    LIMIT ${limit}
  `;
}

async function searchSemantic(sql, query, limit) {
  const queryEmbedding = await embedQuery(query);
  const vector = vectorLiteral(queryEmbedding);

  return sql`
    SELECT file_path, chunk_no, language, content,
           left(content, 600) AS preview,
           1 - (embedding <=> ${vector}::vector) AS similarity
    FROM lofice_code_index
    WHERE embedding IS NOT NULL
    ORDER BY embedding <=> ${vector}::vector
    LIMIT ${limit}
  `;
}

/** @param {URLSearchParams} searchParams */
export async function handleSearch(searchParams) {
  const query = (searchParams.get('q') || '').trim();
  if (!query) {
    return { status: 400, body: { error: 'Missing query parameter q' } };
  }

  const databaseUrl = process.env.DATABASE_URL;
  if (!databaseUrl) {
    return { status: 503, body: { error: 'DATABASE_URL is not configured' } };
  }

  const modeParam = (searchParams.get('mode') || 'semantic').toLowerCase();
  const limit = Math.min(Math.max(Number(searchParams.get('limit') || 6), 1), 12);
  const sql = neon(databaseUrl);

  let mode = modeParam;
  let rows = [];

  if (mode === 'semantic' && process.env.OPENAI_API_KEY?.trim()) {
    try {
      rows = await searchSemantic(sql, query, limit);
    } catch (err) {
      console.error('[rag-search] semantic failed, fallback FTS:', err.message);
      mode = 'fulltext';
    }
  } else if (mode === 'semantic') {
    mode = 'fulltext';
  }

  if (mode === 'fulltext' || rows.length === 0) {
    rows = await searchFullText(sql, query, limit);
    mode = 'fulltext';
  }

  const context = buildContextFromRows(rows);
  return {
    status: 200,
    body: {
      query,
      mode,
      hits: rows.length,
      context,
      results: rows.map((row) => ({
        file_path: row.file_path,
        chunk_no: row.chunk_no,
        language: row.language,
        preview: row.preview ?? row.content?.slice(0, 400),
        score: row.similarity ?? row.rank ?? null,
      })),
    },
  };
}
