set -e

DOCKER_CONTAINER="TPE"
QCOW2_IMAGE="Image/x64BareBonesImage.qcow2"

sudo chmod 666 $QCOW2_IMAGE || true

if ! sudo docker ps | grep -q "$DOCKER_CONTAINER"; then
    sudo docker start $DOCKER_CONTAINER
fi

sudo docker exec -it $DOCKER_CONTAINER make -C /root/Toolchain
sudo docker exec -it $DOCKER_CONTAINER make -C /root/ | tee out.txt

qemu-system-x86_64 -hda $QCOW2_IMAGE -m 512 -audiodev coreaudio,id=snd0 -machine pcspk-audiodev=snd0

sudo docker exec -it $DOCKER_CONTAINER make clean -C /root/Toolchain
sudo docker exec -it $DOCKER_CONTAINER make clean -C /root/

sudo docker stop $DOCKER_CONTAINER