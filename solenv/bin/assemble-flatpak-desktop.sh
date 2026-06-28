#! /bin/bash
#
# This file is part of the lofice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

# Part of solenv/bin/assemble-flatpak.sh that is shared with a downstream mechanism of building a
# Flatpak from a Fedora lofice.spec file.
#
# Arguments:
# $1  pathname, ending in a slash, of the directory containing the source lofice-*.desktop
#     files
# $2  pathname, ending in a slash, of the directory into which to put the target
#     $3.*.desktop files
# $3  the flatpak ID

set -e

## lofice-*.desktop -> $3.*.desktop:
for i in "${1?}"lofice-*.desktop
do
 sed -e "s/^Icon=lofice-/Icon=${3?}./" "$i" \
  >"${2?}${3?}"."${i#"${1?}"lofice-}"
done
mv "${2?}${3?}".startcenter.desktop "${2?}${3?}".desktop

# Flatpak .desktop exports take precedence over system ones due to
# the order of XDG_DATA_DIRS - re-associating text/plain seems a bit much
sed -i "s/text\/plain;//" "${2?}${3?}".writer.desktop

desktop-file-edit --set-key=X-Endless-Alias --set-value=lofice-startcenter \
 --set-key=X-Flatpak-RenamedFrom --set-value='lofice-startcenter.desktop;' \
 "${2?}${3?}".desktop
for i in base calc draw impress math writer xsltfilter
do
 desktop-file-edit --set-key=X-Endless-Alias --set-value=lofice-"$i" \
  --set-key=X-Flatpak-RenamedFrom \
  --set-value="lofice-$i.desktop;${3?}-$i.desktop;" \
  "${2?}${3?}"."$i".desktop
done
