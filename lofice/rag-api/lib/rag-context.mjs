/**
 * lofice — format Neon search hits as LLM context block
 */

export function buildContextFromRows(rows, { maxChars = 6000 } = {}) {
  if (!rows.length)
    return '';

  const parts = [];
  let total = 0;

  for (const row of rows) {
    const body = row.content ?? row.preview ?? '';
    const block =
      `${row.file_path} (chunk ${row.chunk_no}, ${row.language ?? 'text'}):\n${body}`;
    if (total + block.length > maxChars)
      break;
    parts.push(block);
    total += block.length;
  }

  return parts.join('\n\n---\n\n');
}
