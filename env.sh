
# sanity check

if [ ! -e "env.sh" ]; then
    echo "ERROR: this script must be sourced from the ROOT of this repository."
    echo

    return 1
fi

TOPDIR="$(pwd)"

export TOPDIR="$TOPDIR"
