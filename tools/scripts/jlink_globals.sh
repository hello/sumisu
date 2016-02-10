JLINK_DIR_NAME="jlink"
JLINK_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/../$JLINK_DIR_NAME"
JLINK_OPTIONS=" -device nRF52832_xxAA -if swd -speed 4000"
JLINK_CLI="$JLINK_ROOT/JLinkExe.command $JLINK_OPTIONS"
