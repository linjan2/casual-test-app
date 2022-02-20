#!/bin/env sh
export PATH=${CASUAL_HOME}/bin:${PATH}
export LD_LIBRARY_PATH=${CASUAL_HOME}/lib:${LD_LIBRARY_PATH}
umask 0007
exec "$@"
