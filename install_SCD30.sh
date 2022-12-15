#!/bin/bash

    echo ""
    echo ""
    echo "This script will install flowmeter monitoring for reef-pi and is provided by Sral."
    echo ""
    echo ""
    echo ""
    echo ""
    echo "Copyright (c) 2022 Sral"
    echo ""
    echo "Licensed under the Apache License, Version 2.0 (the "License");"
    echo "you may not use this file except in compliance with the License."
    echo "You may obtain a copy of the License at"
    echo ""
    echo "    http://www.apache.org/licenses/LICENSE-2.0"
    echo ""
    echo "Unless required by applicable law or agreed to in writing, software"
    echo "distributed under the License is distributed on an "AS IS" BASIS,"
    echo "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied."
    echo "See the License for the specific language governing permissions and"
    echo "limitations under the License."
    echo ""
    echo ""
    echo "press any key to install..."
    echo ""
    echo ""
    read input
    echo ""
    echo ""

    echo "Stopping service and removing old files ..."
    sudo systemctl stop SCD30.service
    sudo rm -rf /var/lib/reef-pi/SCD30

    echo "Copying new service and setting execution rights..."
    sudo cp src/SCD30.service /etc/systemd/system/SCD30.service
    sudo chmod -R 770 /etc/systemd/system/SCD30.service
    
    # create directory and copy new files
    echo "Creating Reef-Pi directory and copying new files..."
    sudo mkdir /var/lib/reef-pi/SCD30
    sudo cp src/SCD30.cpp          /var/lib/reef-pi/SCD30/SCD30.cpp
    sudo cp src/SCD30FetchData     /var/lib/reef-pi/SCD30/SCD30FetchData
    sudo cp src/SCD30FetchData.cpp /var/lib/reef-pi/SCD30/SCD30FetchData.cpp
    sudo cp src/SCD30Control       /var/lib/reef-pi/SCD30/SCD30Control
    sudo cp src/SCD30Control.cpp   /var/lib/reef-pi/SCD30/SCD30Control.cpp
    # set rights
    sudo chmod -R 777 /var/lib/reef-pi/SCD30/
    
    echo "Deleting install files..."
    sudo rm -r src/
	sudo rm -r LICENSE
	sudo rm -r README.md
    sudo rm SCD30.zip
    sudo rm install_SCD30.sh

    echo "Establishing and starting service..."
    sudo systemctl enable SCD30.service
    sudo systemctl start SCD30.service
    echo ""
    echo ""
    echo "----------------------------------------------------------------------------"
    echo ""
    echo "Installation Complete"
    echo ""
    echo "----------------------------------------------------------------------------"
    echo ""
    echo ""
