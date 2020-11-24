#!/bin/bash
normaluser=$(users | awk '{print $1}')
sudo -H -u $normaluser bash -c '../qgears2/qgears2-1.0.1/qgears' #umm ez amiatt szükséges hogy ne root joggal fusson a benchmark, sajnos nem képes rá

