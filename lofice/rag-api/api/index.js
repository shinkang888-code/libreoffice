export default function handler(_req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Content-Type', 'application/json; charset=utf-8');
  res.status(200).json({
    ok: true,
    service: 'lofice-rag-api',
    provider: 'vercel',
    endpoints: {
      health: '/health',
      search: '/search?q={query}&mode=semantic|fulltext&limit=6',
    },
    docs: 'https://github.com/shinkang888-code/libreoffice/tree/master/lofice/docs/neon-vercel-rag-api.md',
  });
}
