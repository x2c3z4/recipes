#!/bin/bash
set -o nounset          # Treat unset variables as an error

awk '{printf "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",($2+$14)/2,($3+$15)/2,($4+$16)/2,($6+$18)/2,($7+$19)/2,($8+$20)/2,(10+$22)/2,($11+$23)/2,($12+$24)/2}' /tmp/test/xx

