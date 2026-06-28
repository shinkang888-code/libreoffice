#!/usr/bin/env node
/**
 * Sync Neon DATABASE_URL (and optional OPENAI_API_KEY) to Vercel project env.
 *
 * Usage (repo root):
 *   node lofice/scripts/sync-neon-vercel-env.mjs
 *   node lofice/scripts/sync-neon-vercel-env.mjs --deploy
 */
import { execSync } from 'node:child_process';
import { readFileSync, existsSync, writeFileSync, unlinkSync } from 'node:fs';
import { randomBytes } from 'node:crypto';
import { tmpdir } from 'node:os';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const REPO_ROOT = path.resolve(__dirname, '../..');
const RAG_API_DIR = path.join(REPO_ROOT, 'lofice', 'rag-api');
const NEON_PROJECT_ID = process.env.NEON_PROJECT_ID || 'gentle-base-41825553';

const deploy = process.argv.includes('--deploy');
const VERCEL_SCOPE = process.env.VERCEL_SCOPE || 'shinkang888-codes-projects';
const VERCEL_ENVS = ['production'];

function sh(cmd, opts = {}) {
  return execSync(cmd, { encoding: 'utf8', stdio: ['pipe', 'pipe', 'pipe'], ...opts }).trim();
}

function loadEnvLocal() {
  const envPath = path.join(REPO_ROOT, '.env.local');
  if (!existsSync(envPath))
    return {};
  const out = {};
  for (const line of readFileSync(envPath, 'utf8').split('\n')) {
    const m = line.match(/^\s*([^#=]+)=(.*)$/);
    if (!m)
      continue;
    out[m[1].trim()] = m[2].trim().replace(/^"|"$/g, '');
  }
  return out;
}

function getDatabaseUrl() {
  const local = loadEnvLocal();
  if (local.DATABASE_URL)
    return local.DATABASE_URL;
  return sh(`neonctl connection-string --project-id ${NEON_PROJECT_ID} --pooled`);
}

function vercelEnvSet(name, value, environments = VERCEL_ENVS) {
  const tmp = path.join(tmpdir(), `lofice-vercel-${randomBytes(4).toString('hex')}.txt`);
  writeFileSync(tmp, value, 'utf8');
  try {
    for (const env of environments) {
      const cmd = process.platform === 'win32'
        ? `cmd /c type "${tmp}" | vercel env add ${name} ${env} --force --yes --scope ${VERCEL_SCOPE}`
        : `vercel env add ${name} ${env} --force --yes --scope ${VERCEL_SCOPE} < "${tmp}"`;
      sh(cmd, { cwd: RAG_API_DIR, shell: true });
    }
  } finally {
    unlinkSync(tmp);
  }
}

console.log('==> Neon checkout (refresh .env.local)');
try {
  sh(`neonctl checkout main --project-id ${NEON_PROJECT_ID}`, { cwd: REPO_ROOT });
} catch (err) {
  console.warn('WARN: neonctl checkout skipped:', err.message);
}

const databaseUrl = getDatabaseUrl();
if (!databaseUrl)
  throw new Error('DATABASE_URL not found — run neonctl link && neonctl checkout main');

console.log('==> Vercel env: DATABASE_URL');
vercelEnvSet('DATABASE_URL', databaseUrl);

const local = loadEnvLocal();
if (local.OPENAI_API_KEY?.trim()) {
  console.log('==> Vercel env: OPENAI_API_KEY (from .env.local)');
  vercelEnvSet('OPENAI_API_KEY', local.OPENAI_API_KEY.trim());
} else {
  console.log('SKIP: OPENAI_API_KEY not in .env.local (semantic search uses FTS only)');
}

console.log(JSON.stringify({
  ok: true,
  neonProjectId: NEON_PROJECT_ID,
  ragApiDir: RAG_API_DIR,
  synced: ['DATABASE_URL', ...(local.OPENAI_API_KEY ? ['OPENAI_API_KEY'] : [])],
}, null, 2));

if (deploy) {
  console.log('==> vercel deploy --prod');
  const out = sh(`vercel deploy --prod --yes --scope ${VERCEL_SCOPE}`, { cwd: RAG_API_DIR });
  console.log(out);
}
