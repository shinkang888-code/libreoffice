#!/usr/bin/env node
/**
 * lofice — Backfill pgvector embeddings for lofice_code_index (OpenAI)
 *
 * Usage:
 *   $env:OPENAI_API_KEY = "sk-..."
 *   npm run embed
 */

import { neon } from '@neondatabase/serverless';
import { embedTexts, vectorLiteral } from './lib/openai-embeddings.mjs';
import { loadEnvLocal } from './lib/load-env-local.mjs';

loadEnvLocal();

const BATCH_SIZE = 16;
const BATCH_DELAY_MS = 250;

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

async function main() {
  const databaseUrl = process.env.DATABASE_URL;
  if (!databaseUrl) {
    console.error('DATABASE_URL is required.');
    process.exit(1);
  }

  const sql = neon(databaseUrl);
  const pending = await sql`
    SELECT id, content
    FROM lofice_code_index
    WHERE embedding IS NULL
    ORDER BY id
  `;

  if (pending.length === 0) {
    console.log(JSON.stringify({ message: 'All rows already have embeddings.', updated: 0 }, null, 2));
    return;
  }

  let updated = 0;
  for (let i = 0; i < pending.length; i += BATCH_SIZE) {
    const batch = pending.slice(i, i + BATCH_SIZE);
    const embeddings = await embedTexts(batch.map((row) => row.content));

    for (let j = 0; j < batch.length; j++) {
      await sql`
        UPDATE lofice_code_index
        SET embedding = ${vectorLiteral(embeddings[j])}::vector
        WHERE id = ${batch[j].id}
      `;
      updated += 1;
    }

    if (i + BATCH_SIZE < pending.length)
      await sleep(BATCH_DELAY_MS);
  }

  const [{ with_embedding }] = await sql`
    SELECT COUNT(*)::int AS with_embedding
    FROM lofice_code_index
    WHERE embedding IS NOT NULL
  `;

  console.log(JSON.stringify({
    pending: pending.length,
    updated,
    withEmbedding: with_embedding,
  }, null, 2));
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
