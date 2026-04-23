#! /bin/bash
# Remove a container and its image.
# VMO LAB, 2024

########### CONFIGURATIONS ###########

SCRIPT_DIR="$(realpath "$(dirname "$0")")"

# Default args
directory_name="$SCRIPT_DIR/../advcmp"
container_name=advcmp
image_name=advcmp

######################################

########### CLI OPTIONS ##############

function usage() {
    echo "Usage: ./deploy.sh [OPTION]..."
    echo "Cleanup the image, container and build directory for the advanced compiler project."
    echo ""
    echo "Options:"
    echo "  -h, --help          Print this message"
    echo "  -d, --dir=PATH      Path of a directory where build directory is located in"
    echo "  -n, --name=NAME     Container name"
    echo "  -i, --image=IMAGE   Image name"
}

# Get options from command
OPT=d:n:i:h::
LOPT=dir:,name:,image:,help::

PARSED=$(getopt --options "$OPT", --longoptions "$LOPT" --name "$0" -- "$@")
if [[ $? -ne 0 ]]; then
    exit 1
fi
eval set -- "$PARSED"

while true; do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        -d|--dir)
            directory_name="$2"
            shift 2
            ;;
        -n|--name)
            container_name="$2"
            shift 2
            ;;
        -i|--image)
            image_name="$2"
            shift 2
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Error"
            exit 1
            ;;
    esac
done

######################################

docker rm -f "$container_name"
docker rmi "$image_name"
rm -rf "$directory_name/build"
