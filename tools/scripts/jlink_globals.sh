JLINK_DIR_NAME="jlink"
JLINK_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/../$JLINK_DIR_NAME"
JLINK_CLI="$JLINK_ROOT/JLinkExe.command -device nRF52832_xxAA -if swd -speed 4000"
