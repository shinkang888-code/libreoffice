#!/usr/bin/env node
/**
 * lofice — local RAG search HTTP server (Neon FTS / semantic)
 *
 * Usage:
 *   npm run rag-server
 *   GET /search?q=<query>&mode=semantic|fulltext&limit=6
 */

import http from 'node:http';
import { URL } from 'node:url';
import { handleSearch } from './lib/search-core.mjs';

const PORT = Number(process.env.LOFICE_RAG_PORT || 8787);
const HOST = process.env.LOFICE_RAG_HOST || '127.0.0.1';

const server = http.createServer(async (req, res) => {
  try {
    const url = new URL(req.url || '/', `http://${HOST}:${PORT}`);

    if (req.method === 'GET' && url.pathname === '/health') {
      res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify({ ok: true, provider: 'local' }));
      return;
    }

    if (req.method === 'GET' && url.pathname === '/search') {
      const { status, body } = await handleSearch(url.searchParams);
      res.writeHead(status, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify(body));
      return;
    }

    res.writeHead(404, { 'Content-Type': 'application/json; charset=utf-8' });
    res.end(JSON.stringify({ error: 'Not found' }));
  } catch (err) {
    console.error('[rag-server]', err);
    res.writeHead(500, { 'Content-Type': 'application/json; charset=utf-8' });
    res.end(JSON.stringify({ error: err.message || 'Internal error' }));
  }
});

server.listen(PORT, HOST, () => {
  console.log(JSON.stringify({
    message: 'lofice RAG search server running',
    host: HOST,
    port: PORT,
    endpoints: ['/health', '/search?q=...&mode=semantic|fulltext&limit=6'],
  }, null, 2));
});
