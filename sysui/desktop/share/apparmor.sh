#!/usr/bin/env bash
# This file is part of the lofice project.
# ------------------------------------------------------------------
#
#    Copyright (C) 2016 Canonical Ltd.
#
#    This Source Code Form is subject to the terms of the Mozilla Public
#    License, v. 2.0. If a copy of the MPL was not distributed with this
#    file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
#    Author: Bryan Quigley <bryan.quigley@canonical.com>
#
# ------------------------------------------------------------------

# This is a simple script to help get AppArmor working on different distros
# Generally these apparmor profiles target the latest lofice

INST_ROOT=$1  #Where lofice program folder can be found
PROFILESFROM=$2  #Where the profile files are
INSTALLTO=$3  #Where should the apparmor profiles (For manual use should be /etc/apparmor.d)
RESTART=$4 #Should we restart apparmor using service?
CHECK=$5 #Check parsing of the new profile?

#Example uses:
#Ubuntu 16.04 with stock lofice:
# sudo ./sysui/desktop/share/apparmor.sh /usr/lib/lofice/ sysui/desktop/apparmor/ /etc/apparmor.d/ true true

#Ubuntu 16.04, with built debs from lofice git
# sudo ./sysui/desktop/share/apparmor.sh /opt/loficeDev5.2/ sysui/desktop/apparmor/ /etc/apparmor.d/ true true

#Ubuntu 16.04, running from git!
# sudo ./sysui/desktop/share/apparmor.sh /mnt/store/git/libo/instdir/ sysui/desktop/apparmor/ /etc/apparmor.d/ true true

for filename in $PROFILESFROM/*
do
    [[ -e $filename ]] || { echo "No profile files found in ""$PROFILESFROM"; exit 1; }
    tourl=${INSTALLTO%/}/${filename##*/}
    sed "s!@INSTDIR@!${INST_ROOT%/}!g" "$filename" > "$tourl"
    echo "$tourl"
  if [ "$CHECK" = "true" ]; then
    # check profile parsing
    echo "Checking $tourl profile."
    /sbin/apparmor_parser --add --skip-cache --skip-kernel-load $tourl
  fi
done

if [ "$RESTART" = true ] ; then
    echo "Restarting AppArmor"
    service apparmor restart
fi
