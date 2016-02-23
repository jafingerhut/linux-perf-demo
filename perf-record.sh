#! /bin/bash

PID="$1"

set -ex
perf record --call-graph fp --pid=$PID
