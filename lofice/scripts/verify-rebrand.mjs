#!/usr/bin/env node
/**
 * lofice — rebranding verification (productlist, icons, instsetoo, module samples)
 */
import { access, readFile } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const REPO_ROOT = path.resolve(__dirname, '../..');

async function readRepo(rel) {
  return readFile(path.join(REPO_ROOT, rel), 'utf8');
}

async function fileExists(rel) {
  try {
    await access(path.join(REPO_ROOT, rel));
    return true;
  } catch {
    return false;
  }
}

function countLibreOffice(text) {
  return (text.match(/LibreOffice/g) ?? []).length;
}

const checks = [];

async function check(name, fn) {
  try {
    const detail = await fn();
    checks.push({ name, ok: detail.ok, ...detail });
  } catch (err) {
    checks.push({ name, ok: false, error: String(err.message ?? err) });
  }
}

await check('productlist', async () => {
  const text = await readRepo('sysui/productlist.mk');
  const ok =
    text.includes('PRODUCTNAME.lofice := lofice')
    && text.includes('PRODUCTLIST := lofice');
  return { ok, detail: ok ? 'lofice product configured' : 'missing lofice product entries' };
});

await check('icon-mainapp-svg', async () => {
  const text = await readRepo('icon-themes/colibre_svg/res/mainapp_16.svg');
  const ok = text.includes('lofice_16.png');
  return { ok, detail: ok ? 'mainapp embeds lofice PNG' : 'mainapp SVG not rebranded' };
});

await check('icon-mainapp-png', async () => {
  const ok = await fileExists('icon-themes/colibre_svg/res/lofice_16.png');
  return { ok, detail: ok ? 'lofice_16.png deployed' : 'missing lofice_16.png in colibre_svg' };
});

await check('instsetoo-archive-paths', async () => {
  const text = await readRepo('instsetoo_native/CustomTarget_install.mk');
  const ok =
    text.includes('lofice*_archive.zip')
    && !text.includes('LibreOffice*_archive');
  return { ok, detail: ok ? 'test install uses lofice archives' : 'LibreOffice archive paths remain' };
});

await check('framework-menubar-header', async () => {
  const text = await readRepo('framework/uiconfig/startmodule/menubar/menubar.xml');
  const ok = text.includes('lofice project') && !text.includes('LibreOffice project');
  return { ok, detail: ok ? 'framework menubar header rebranded' : 'LibreOffice header in framework menubar' };
});

await check('sw-menubar-header', async () => {
  const text = await readRepo('sw/uiconfig/swriter/menubar/menubar.xml');
  const ok = text.includes('lofice project') && !text.includes('LibreOffice project');
  return { ok, detail: ok ? 'sw menubar header rebranded' : 'LibreOffice header in sw menubar' };
});

await check('msi-property-ulf', async () => {
  const text = await readRepo('instsetoo_native/inc_openoffice/windows/msi_languages/Property.ulf');
  const libreCount = countLibreOffice(text);
  const ok = text.includes('lofice') && libreCount === 0;
  return { ok, libreCount, detail: ok ? 'MSI Property.ulf uses lofice' : `LibreOffice mentions: ${libreCount}` };
});

await check('cui-about-product', async () => {
  const text = await readRepo('cui/source/dialogs/about.cxx');
  const ok = text.includes('"lofice"') && !text.includes('LibreOffice');
  return { ok, detail: ok ? 'About dialog uses lofice product name' : 'LibreOffice still in about.cxx' };
});

await check('officecfg-lofice-xcu', async () => {
  const ok = await fileExists('officecfg/registry/data/org/openoffice/Office/Lofice.xcu');
  return { ok, detail: ok ? 'Lofice.xcu present' : 'missing Lofice.xcu' };
});

const ok = checks.every((c) => c.ok);
console.log(JSON.stringify({ ok, checks }, null, 2));
if (!ok)
  process.exit(1);
