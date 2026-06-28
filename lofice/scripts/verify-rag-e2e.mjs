#!/usr/bin/env node
/**
 * lofice — RAG end-to-end probe (matches AiRagContext.cxx URL shape)
 */
import { pathToFileURL } from 'node:url';

const DEFAULT_ENDPOINT = 'https://lofice-rag-api.vercel.app/search';
const TEST_QUERY = 'AiAssistantPanel quick action auto send';
const TIMEOUT_MS = 30000;

/** Same query string as AiRagContext::buildSearchUrl */
function buildSearchUrl(endpoint, query) {
  const sep = endpoint.includes('?') ? '&' : '?';
  return `${endpoint}${sep}q=${encodeURIComponent(query)}&mode=semantic&limit=6`;
}

export async function verifyRagE2e(
  endpoint = process.env.LOFICE_RAG_ENDPOINT || DEFAULT_ENDPOINT,
  query = TEST_QUERY,
) {
  const url = buildSearchUrl(endpoint, query);
  const controller = new AbortController();
  const timer = setTimeout(() => controller.abort(), TIMEOUT_MS);

  try {
    const res = await fetch(url, {
      signal: controller.signal,
      headers: { Accept: 'application/json' },
    });
    const body = await res.json();

    const context = typeof body.context === 'string' ? body.context : '';
    const ok = res.ok
      && !body.error
      && body.mode === 'semantic'
      && (body.hits ?? 0) > 0
      && context.length > 0
      && context.includes('lofice/');

    return {
      ok,
      url,
      status: res.status,
      mode: body.mode,
      hits: body.hits ?? 0,
      contextChars: context.length,
      preview: context.slice(0, 120),
    };
  } catch (err) {
    return {
      ok: false,
      url: buildSearchUrl(endpoint, query),
      error: err instanceof Error ? err.message : String(err),
    };
  } finally {
    clearTimeout(timer);
  }
}

const isMain = process.argv[1]
  && import.meta.url === pathToFileURL(process.argv[1]).href;

if (isMain) {
  const result = await verifyRagE2e();
  console.log(JSON.stringify(result, null, 2));
  if (!result.ok)
    process.exit(1);
}
