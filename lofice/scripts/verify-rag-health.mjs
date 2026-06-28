#!/usr/bin/env node
/**
 * lofice — RAG API health probe (Vercel production default)
 */
import { pathToFileURL } from 'node:url';

const DEFAULT_URL = 'https://lofice-rag-api.vercel.app/health';
const TIMEOUT_MS = 15000;

export async function verifyRagHealth(url = process.env.LOFICE_RAG_HEALTH_URL || DEFAULT_URL) {
  const controller = new AbortController();
  const timer = setTimeout(() => controller.abort(), TIMEOUT_MS);
  try {
    const res = await fetch(url, { signal: controller.signal });
    const body = await res.text();
    let json = null;
    try {
      json = JSON.parse(body);
    } catch {
      json = { raw: body.slice(0, 200) };
    }
    return {
      ok: res.ok && json?.ok === true,
      url,
      status: res.status,
      body: json,
    };
  } catch (err) {
    return {
      ok: false,
      url,
      error: err instanceof Error ? err.message : String(err),
    };
  } finally {
    clearTimeout(timer);
  }
}

const isMain = process.argv[1]
  && import.meta.url === pathToFileURL(process.argv[1]).href;

if (isMain) {
  const result = await verifyRagHealth();
  console.log(JSON.stringify(result, null, 2));
  if (process.env.LOFICE_CI_STRICT_RAG === '1' && !result.ok)
    process.exit(1);
}
