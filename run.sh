#!/bin/bash
make all

if [ $# -ne 1 ]; then
  echo "Usage: $0 <parameter>"
  exit 1
fi

case "$1" in
  "all")
    ./application.out | ./vista.out &
    ;;
  "app")
    ./application.out
    ;;
  "vista")
      ./vista.out
      ;;
  *)
    echo "Invalid parameter: $1"
    exit 1
    ;;
esac

make clean