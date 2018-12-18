#!/bin/bash
mkdir -p /home/$USER/
cp -a Proyecto/ /home/$USER/
make -C /home/$USER/Proyecto/cliente/
make -C /home/$USER/Proyecto/servidor/
echo -e "export PATH=$PATH:/home/$USER/Proyecto/cliente/:/home/$USER/Proyecto/servidor/\n" >> ~/.bashrc
cp -a szclient.6.gz /usr/share/man/man6/
cp -a szserver.6.gz /usr/share/man/man6/
cp -a uninstall.sh /home/$USER/Proyecto/
chmod -R 777 /home/$USER/Proyecto/
mandb

