#!/bin/bash

SPOOL_DIR=spool

shopt -s nullglob
for f in $SPOOL_DIR/*.pid; do
  name=$(echo `basename $f` | sed s/.pid//)
  pid=$(cat $f)
  echo "Stopping printer $name (pid $pid)"
  kill $pid
done
