#!/bin/bash

for sim in /home/mbeuschlein/utr/macros/angular_efficiency_116Sn_target_tilted_5deg/angdist*.xmac; do
	echo 'Simulating ${sim}'
	OutputProcessing/utrwrapper.py ${sim}

done
