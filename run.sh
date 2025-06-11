#!/bin/bash

CONTAINER_NAME=tpe_grupo1

# Verificar si el contenedor ya existe
if [ "$(docker ps -a -q -f name=$CONTAINER_NAME)" ]; then
    echo "El contenedor $CONTAINER_NAME ya existe. Reutilizándolo..."
    docker start $CONTAINER_NAME
else
    echo "Creando un nuevo contenedor $CONTAINER_NAME..."
    docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -it --name $CONTAINER_NAME agodio/itba-so:2.0
fi


echo "Limpiando compilaciones anteriores..."
docker exec -it $CONTAINER_NAME make clean -C /root/Toolchain
docker exec -it $CONTAINER_NAME make clean -C /root

echo "Compilando Toolchain..."
docker exec -it $CONTAINER_NAME make -C /root/Toolchain

echo "Compilando proyecto completo..."
docker exec -it $CONTAINER_NAME make -C /root

sudo chown $USER:$USER Image/x64BareBonesImage.qcow2

if [ "$(uname)" == "Darwin" ]; then
    audio_driver="coreaudio"
else
    audio_driver="sdl"
fi

qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2  -m 512 -audiodev ${audio_driver},id=speaker -machine pcspk-audiodev=speaker

echo "Limpiando..."
docker exec -it $CONTAINER_NAME make clean -C /root/Toolchain
docker exec -it $CONTAINER_NAME make clean -C /root

echo "Deteniendo contenedor..."
docker stop $CONTAINER_NAME