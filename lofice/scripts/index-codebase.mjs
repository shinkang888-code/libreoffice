#!/usr/bin/env node
/**
 * lofice — Neon codebase indexer
 * Indexes lofice/ sources into Neon Postgres (lofice_code_index).
 *
 * Usage:
 *   $env:DATABASE_URL = (neonctl connection-string --project-id gentle-base-41825553)
 *   npm run index
 */

import { createHash } from 'node:crypto';
import { readdir, readFile, stat } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { neon } from '@neondatabase/serverless';
import { loadEnvLocal } from './lib/load-env-local.mjs';

loadEnvLocal();

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const LOFICE_ROOT = path.resolve(__dirname, '..');
const REPO_ROOT = path.resolve(LOFICE_ROOT, '..');

const INCLUDE_EXTENSIONS = new Set([
  '.cxx', '.hxx', '.md', '.ui', '.mk', '.json', '.sql', '.ps1',
]);
const SKIP_DIRS = new Set([
  'node_modules', '.git', 'workdir', 'instdir', 'config_host',
]);
const CHUNK_SIZE = 1800;
const CHUNK_OVERLAP = 200;

function languageFromExt(ext) {
  switch (ext) {
    case '.cxx': return 'cpp';
    case '.hxx': return 'cpp-header';
    case '.md': return 'markdown';
    case '.ui': return 'ui';
    case '.mk': return 'makefile';
    case '.json': return 'json';
    case '.sql': return 'sql';
    case '.ps1': return 'powershell';
    default: return ext.slice(1) || 'text';
  }
}

function sha256(text) {
  return createHash('sha256').update(text, 'utf8').digest('hex');
}

function chunkText(text) {
  if (text.length <= CHUNK_SIZE)
    return [text];

  const chunks = [];
  let start = 0;
  while (start < text.length) {
    const end = Math.min(text.length, start + CHUNK_SIZE);
    chunks.push(text.slice(start, end));
    if (end >= text.length)
      break;
    start = Math.max(0, end - CHUNK_OVERLAP);
  }
  return chunks;
}

async function walk(dir, out) {
  const entries = await readdir(dir, { withFileTypes: true });
  for (const entry of entries) {
    if (SKIP_DIRS.has(entry.name))
      continue;

    const full = path.join(dir, entry.name);
    if (entry.isDirectory()) {
      await walk(full, out);
      continue;
    }

    const ext = path.extname(entry.name).toLowerCase();
    if (!INCLUDE_EXTENSIONS.has(ext))
      continue;

    const st = await stat(full);
    if (!st.isFile() || st.size > 512_000)
      continue;

    out.push(full);
  }
}

async function collectFiles() {
  const files = [];
  await walk(LOFICE_ROOT, files);
  return files.sort();
}

async function main() {
  const databaseUrl = process.env.DATABASE_URL;
  if (!databaseUrl) {
    console.error('DATABASE_URL is required. Example:');
    console.error('  neonctl connection-string --project-id gentle-base-41825553');
    process.exit(1);
  }

  const sql = neon(databaseUrl);
  const files = await collectFiles();
  let inserted = 0;
  let skipped = 0;

  for (const absPath of files) {
    const relPath = path.relative(REPO_ROOT, absPath).replace(/\\/g, '/');
    const content = await readFile(absPath, 'utf8');
    const language = languageFromExt(path.extname(absPath).toLowerCase());
    const chunks = chunkText(content);

    await sql`DELETE FROM lofice_code_index WHERE file_path = ${relPath}`;

    for (let i = 0; i < chunks.length; i++) {
      const chunk = chunks[i];
      const hash = sha256(chunk);

      const rows = await sql`
        INSERT INTO lofice_code_index (file_path, chunk_no, language, content, content_sha256)
        VALUES (${relPath}, ${i}, ${language}, ${chunk}, ${hash})
        ON CONFLICT (file_path, chunk_no, content_sha256) DO NOTHING
        RETURNING id
      `;

      if (rows.length > 0)
        inserted += 1;
      else
        skipped += 1;
    }
  }

  const [{ count }] = await sql`SELECT COUNT(*)::int AS count FROM lofice_code_index`;
  console.log(JSON.stringify({
    project: 'loice',
    files: files.length,
    inserted,
    skipped,
    totalRows: count,
    root: 'lofice/',
  }, null, 2));
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
