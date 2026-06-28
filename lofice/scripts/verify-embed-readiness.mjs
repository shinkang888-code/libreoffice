#!/usr/bin/env node
/**
 * lofice — Run semantic embed when OPENAI_API_KEY is available; skip otherwise.
 */
import { loadEnvLocal } from './lib/load-env-local.mjs';
import { execSync } from 'node:child_process';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const __dirname = path.dirname(fileURLToPath(import.meta.url));

loadEnvLocal();

const key = process.env.OPENAI_API_KEY?.trim();
if (!key) {
  console.log(JSON.stringify({
    ok: true,
    skipped: true,
    reason: 'OPENAI_API_KEY not set — add to repo-root .env.local then npm run embed',
  }, null, 2));
  process.exit(0);
}

try {
  const out = execSync('node embed-codebase.mjs', {
    cwd: __dirname,
    encoding: 'utf8',
    stdio: ['pipe', 'pipe', 'pipe'],
  });
  console.log(out);
  console.log(JSON.stringify({ ok: true, skipped: false }, null, 2));
} catch (err) {
  console.error(err.stderr?.toString() || err.message);
  process.exit(1);
}
