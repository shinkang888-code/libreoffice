/**
 * lofice — OpenAI text embeddings helper (text-embedding-3-small, 1536 dims)
 */

const DEFAULT_MODEL = 'text-embedding-3-small';

export function requireOpenAiApiKey() {
  const apiKey = process.env.OPENAI_API_KEY?.trim();
  if (!apiKey) {
    throw new Error(
      'OPENAI_API_KEY is required. Set it in .env.local or the environment.',
    );
  }
  return apiKey;
}

export function vectorLiteral(values) {
  return `[${values.join(',')}]`;
}

export async function embedTexts(texts, { model = DEFAULT_MODEL } = {}) {
  if (!texts.length)
    return [];

  const apiKey = requireOpenAiApiKey();
  const res = await fetch('https://api.openai.com/v1/embeddings', {
    method: 'POST',
    headers: {
      Authorization: `Bearer ${apiKey}`,
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({ model, input: texts }),
  });

  if (!res.ok) {
    const body = await res.text();
    throw new Error(`OpenAI embeddings failed (${res.status}): ${body}`);
  }

  const data = await res.json();
  return data.data
    .sort((a, b) => a.index - b.index)
    .map((row) => row.embedding);
}

export async function embedQuery(text, options) {
  const [embedding] = await embedTexts([text], options);
  return embedding;
}
