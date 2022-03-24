#!/bin/bash
sudo service network-manager stop
sudo rm /var/lib/NetworkManager/NetworkManager.state
sudo service network-manager start
sudo gedit /etc/NetworkManager/NetworkManager.conf
#false -> true
#sudo service network-manager restart
