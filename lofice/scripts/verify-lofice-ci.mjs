#!/usr/bin/env node
/**
 * lofice — CI verification bundle (mk, sources, officecfg, ui)
 */
import { access, readFile, readdir } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { verifyRagHealth } from './verify-rag-health.mjs';
import { verifyRagE2e } from './verify-rag-e2e.mjs';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const LOFICE_ROOT = path.resolve(__dirname, '..');
const REPO_ROOT = path.resolve(LOFICE_ROOT, '..');

const REQUIRED_OFFICECFG_PROPS = [
  'UseBuiltinAi',
  'Endpoint',
  'ApiKey',
  'Model',
  'TimeoutSeconds',
  'ChatHistoryJson',
  'UseRagContext',
  'RagEndpoint',
  'RagTimeoutSeconds',
];

const REQUIRED_UI = [
  'uiconfig/modules/lofice/ui/sidebar_ai_assistant.ui',
  'uiconfig/modules/lofice/ui/opt_lofice_ai_page.ui',
];

async function exists(relFromLofice) {
  try {
    await access(path.join(LOFICE_ROOT, relFromLofice));
    return true;
  } catch {
    return false;
  }
}

async function collectCxx(relDir) {
  const abs = path.join(LOFICE_ROOT, relDir);
  const names = await readdir(abs);
  return names
    .filter((name) => name.endsWith('.cxx'))
    .map((name) => ({
      mkEntry: `lofice/${relDir}/${name.replace(/\.cxx$/, '')}`,
      file: `${relDir}/${name}`,
    }));
}

async function verifyMk() {
  const mk = await readFile(path.join(LOFICE_ROOT, 'Library_lofice.mk'), 'utf8');
  const listed = new Set(
    [...mk.matchAll(/lofice\/[^\s\\]+/g)].map((m) => m[0].replace(/\\/g, '')),
  );
  const sources = [
    ...(await collectCxx('source/ai')),
    ...(await collectCxx('source/core')),
    ...(await collectCxx('source/ui')),
  ];
  const missing = sources.filter((s) => !listed.has(s.mkEntry));
  const missingFiles = [];
  for (const s of sources) {
    if (!(await exists(s.file)))
      missingFiles.push(s.file);
  }
  return { missing, missingFiles, count: sources.length };
}

async function verifyOfficecfg() {
  const xcs = await readFile(
    path.join(REPO_ROOT, 'officecfg/registry/schema/org/openoffice/Office/Lofice.xcs'),
    'utf8',
  );
  const xcu = await readFile(
    path.join(REPO_ROOT, 'officecfg/registry/data/org/openoffice/Office/Lofice.xcu'),
    'utf8',
  );
  const missingInXcs = REQUIRED_OFFICECFG_PROPS.filter((p) => !xcs.includes(`oor:name="${p}"`));
  const missingInXcu = REQUIRED_OFFICECFG_PROPS.filter((p) => !xcu.includes(`oor:name="${p}"`));
  return { missingInXcs, missingInXcu };
}

async function verifyUi() {
  const missing = [];
  for (const rel of REQUIRED_UI) {
    if (!(await exists(rel)))
      missing.push(rel);
  }
  return { missing };
}

const REQUIRED_UI_WIDGETS = [
  'btn_export_history',
  'btn_import_history',
];

async function verifyUiWidgets() {
  const uiPath = path.join(LOFICE_ROOT, 'uiconfig/modules/lofice/ui/sidebar_ai_assistant.ui');
  const ui = await readFile(uiPath, 'utf8');
  const missing = REQUIRED_UI_WIDGETS.filter((id) => !ui.includes(`id="${id}"`));
  return { missing };
}

const mk = await verifyMk();
const officecfg = await verifyOfficecfg();
const ui = await verifyUi();
const uiWidgets = await verifyUiWidgets();
const rag = await verifyRagHealth();
const ragE2e = await verifyRagE2e();

let rebrand = { ok: true, skipped: true };
try {
  const { spawnSync } = await import('node:child_process');
  const result = spawnSync(process.execPath, ['verify-rebrand.mjs'], {
    cwd: __dirname,
    encoding: 'utf8',
  });
  rebrand = JSON.parse(result.stdout || '{"ok":false}');
} catch {
  rebrand = { ok: false, error: 'verify-rebrand failed to run' };
}

const ok =
  mk.missing.length === 0
  && mk.missingFiles.length === 0
  && officecfg.missingInXcs.length === 0
  && officecfg.missingInXcu.length === 0
  && ui.missing.length === 0
  && uiWidgets.missing.length === 0
  && rebrand.ok;

console.log(JSON.stringify({ ok, mk, officecfg, ui, uiWidgets, rag, ragE2e, rebrand }, null, 2));
if (!ok)
  process.exit(1);
