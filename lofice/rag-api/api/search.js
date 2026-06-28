import { handleSearch } from '../lib/search-core.mjs';

function setCors(res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');
}

export default async function handler(req, res) {
  setCors(res);

  if (req.method === 'OPTIONS')
    return res.status(204).end();

  if (req.method !== 'GET')
    return res.status(405).json({ error: 'Method not allowed' });

  try {
    const host = req.headers['x-forwarded-host'] || req.headers.host || 'localhost';
    const proto = req.headers['x-forwarded-proto'] || 'https';
    const url = new URL(req.url || '/search', `${proto}://${host}`);
    const { status, body } = await handleSearch(url.searchParams);
    return res.status(status).json(body);
  } catch (err) {
    console.error('[rag-api/search]', err);
    return res.status(500).json({ error: err.message || 'Internal error' });
  }
}
