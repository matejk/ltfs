#!/bin/sh
# readdirplus: listing a directory must return correct attributes and,
# on FUSE 3, must not trigger a getattr request per entry.
. "${top_srcdir}/tests/lib/harness.sh"

LTFS_MOUNT_OPTS="-o verbose=6"

NFILES=100

ltfs_setup

mkdir "$MNT/big"
i=0
while [ $i -lt $NFILES ]; do
	head -c $((i + 1)) /dev/zero >"$MNT/big/f$i"
	i=$((i + 1))
done

# Remount so the listing below runs against a cold kernel cache
ltfs_remount

# Attributes reported by the listing must match per-file stat
ls -l "$MNT/big" >"$WORK/listing"
for n in 0 57 99; do
	ls_size=$(awk -v f="f$n" '$NF == f {print $5}' "$WORK/listing")
	[ "$ls_size" = "$((n + 1))" ] || fail "listing reports size $ls_size for f$n"
	stat_size=$(stat -c %s "$MNT/big/f$n")
	[ "$stat_size" = "$((n + 1))" ] || fail "stat reports size $stat_size for f$n"
done

ltfs_finish

# "FUSE getattr/fgetattr" debug lines from the remounted instance show how
# many attribute requests the listing needed
getattrs=$(grep -c "FUSE f*getattr" "$WORK/ltfs-remount.log" || true)
echo "getattr requests during ls -l of $NFILES files: $getattrs"

if ltfs_is_fuse3; then
	# readdirplus delivers attributes with the listing; without it the
	# kernel issues one getattr (via lookup) per entry
	[ "$getattrs" -lt $((NFILES / 2)) ] \
		|| fail "expected readdirplus to suppress per-entry getattr, saw $getattrs"
fi

echo "PASS"
