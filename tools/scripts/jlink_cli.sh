#!/bin/sh
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "$DIR/jlink_globals.sh"

echo "$JLINK_ROOT"
$JLINK_CLI

