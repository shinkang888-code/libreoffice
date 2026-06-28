#!/usr/bin/env node
/**
 * lofice — verify Library_lofice.mk lists all source objects
 */
import { readFile, readdir } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const LOFICE_ROOT = path.resolve(__dirname, '..');
const MK_FILE = path.join(LOFICE_ROOT, 'Library_lofice.mk');

async function collectCxx(relDir) {
  const abs = path.join(LOFICE_ROOT, relDir);
  const names = await readdir(abs);
  return names
    .filter((name) => name.endsWith('.cxx'))
    .map((name) => `lofice/${relDir}/${name.replace(/\.cxx$/, '')}`);
}

const mk = await readFile(MK_FILE, 'utf8');
const listed = new Set(
  [...mk.matchAll(/lofice\/[^\s\\]+/g)].map((m) => m[0].replace(/\\/g, '')),
);

const expected = [
  ...(await collectCxx('source/ai')),
  ...(await collectCxx('source/core')),
  ...(await collectCxx('source/ui')),
].sort();

const missing = expected.filter((item) => !listed.has(item));
const extra = [...listed].filter((item) => item.startsWith('lofice/source/') && !expected.includes(item));

const ok = missing.length === 0;
console.log(JSON.stringify({
  ok,
  mkFile: path.relative(LOFICE_ROOT, MK_FILE),
  expectedCount: expected.length,
  missing,
  extra,
}, null, 2));

if (!ok)
  process.exit(1);
