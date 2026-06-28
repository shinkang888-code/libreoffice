import { existsSync, readFileSync } from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const REPO_ROOT = path.resolve(__dirname, '../../..');

/**
 * Load repo-root `.env.local` into process.env (only keys not already set).
 * Strips surrounding quotes from values.
 */
export function loadEnvLocal() {
  const envPath = path.join(REPO_ROOT, '.env.local');
  if (!existsSync(envPath))
    return {};

  const out = {};
  for (const line of readFileSync(envPath, 'utf8').split('\n')) {
    const m = line.match(/^\s*([^#=]+)=(.*)$/);
    if (!m)
      continue;
    const key = m[1].trim();
    let value = m[2].trim().replace(/^"|"$/g, '');
    if (key === 'DATABASE_URL' || key === 'DATABASE_URL_UNPOOLED')
      value = sanitizeNeonDatabaseUrl(value);
    out[key] = value;
    process.env[key] = value;
  }
  return out;
}

/** @neon/serverless rejects channel_binding=require in some Node builds */
function sanitizeNeonDatabaseUrl(url) {
  try {
    const parsed = new URL(url);
    parsed.searchParams.delete('channel_binding');
    return parsed.toString();
  } catch {
    return url.replace(/([?&])channel_binding=[^&]*&?/g, '$1').replace(/[?&]$/, '');
  }
}

export { REPO_ROOT };
