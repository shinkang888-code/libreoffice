#!/usr/bin/env node
/**
 * lofice — Neon codebase search (full-text or semantic)
 *
 * Usage:
 *   npm run search -- "AiAssistantPanel streaming"
 *   npm run search -- --semantic "how does conversation history persist"
 */

import { neon } from '@neondatabase/serverless';
import { embedQuery, vectorLiteral } from './lib/openai-embeddings.mjs';
import { loadEnvLocal } from './lib/load-env-local.mjs';

loadEnvLocal();

const args = process.argv.slice(2);
const semantic = args.includes('--semantic');
const query = args.filter((arg) => arg !== '--semantic').join(' ').trim();

if (!query) {
  console.error('Usage: npm run search -- [--semantic] "<query>"');
  process.exit(1);
}

const databaseUrl = process.env.DATABASE_URL;
if (!databaseUrl) {
  console.error('DATABASE_URL is required.');
  process.exit(1);
}

const sql = neon(databaseUrl);

if (semantic) {
  const queryEmbedding = await embedQuery(query);
  const vector = vectorLiteral(queryEmbedding);

  const rows = await sql`
    SELECT file_path, chunk_no, language,
           left(content, 400) AS preview,
           1 - (embedding <=> ${vector}::vector) AS similarity
    FROM lofice_code_index
    WHERE embedding IS NOT NULL
    ORDER BY embedding <=> ${vector}::vector
    LIMIT 12
  `;

  console.log(JSON.stringify({
    mode: 'semantic',
    query,
    hits: rows.length,
    results: rows,
  }, null, 2));
} else {
  const rows = await sql`
    SELECT file_path, chunk_no, language,
           left(content, 400) AS preview,
           ts_rank(search_vector, websearch_to_tsquery('simple', ${query})) AS rank
    FROM lofice_code_index
    WHERE search_vector @@ websearch_to_tsquery('simple', ${query})
    ORDER BY rank DESC
    LIMIT 12
  `;

  console.log(JSON.stringify({
    mode: 'fulltext',
    query,
    hits: rows.length,
    results: rows,
  }, null, 2));
}
