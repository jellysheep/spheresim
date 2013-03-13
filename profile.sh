#!/bin/bash
make && (rm -f callgrind.out.*; valgrind --tool=callgrind ./SphereSim & sleep 15 && kill $(ps aux | grep valgrind | grep -v grep | gawk "{printf \"%s\n\",\$2}"))