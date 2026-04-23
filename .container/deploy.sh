#!/bin/bash
# Build Dockerfile and launch a container.
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
    echo "Deploy a container for the advanced compiler project."
    echo ""
    echo "Options:"
    echo "  -h, --help          Print this message"
    echo "  -d, --dir=PATH      Path of a directory to be bind mounted on the container"
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
            echo "Error" 1>&2
            exit 1
            ;;
    esac
done

######################################

mkdir -p "$directory_name"

if [ ! "$(docker ps -a -q -f name=^$container_name$)" ]; then
    if [[ "$(docker images -q $image_name 2> /dev/null)" == "" ]]; then
        if [ -f "$image_name.tar.gz" ]; then
            # Load pre-built image
            echo -n "Importing image..."
            docker image import $image_name.tar.gz $container_name
            echo "    [DONE]"
        else
            # Build container image
            echo -n "Building image..."
            docker build \
                --build-arg UID=$(id -u) \
                --build-arg GID=$(id -g) \
                -t $image_name $SCRIPT_DIR
            echo "Building image...    [DONE]"
        fi
    fi

    docker run \
        -v "$directory_name:/home/ubuntu/$(basename "$(realpath "$directory_name")")" \
        -h "$container_name" --name "$container_name" -it --user ubuntu -d \
        "$image_name" \
        tail -f /dev/null

else
    echo "$container_name is already running." 1>&2
    exit 1
fi
