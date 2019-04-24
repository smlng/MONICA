#!/usr/bin/python3

import logging
import os
import sys

from time import sleep
from OGCDummySensor import create_ogc_dummy_sensor


def main():
    logging.basicConfig(level=logging.DEBUG)
    if len(sys.argv) < 3:
        print("USAGE: {} CSV_FILE INTERVAL".format(sys.argv[0]))
        sys.exit(1)
    if not os.path.isfile(sys.argv[1]):
        print("Invalid CSV_FILE")
        sys.exit(2)

    interval = int(sys.argv[2])
    dummy_sensors = list()

    with open(sys.argv[1]) as csv:
        streams = csv.readlines()
        for s in streams:
            type, url, init = s.strip().split(";")
            dummy_sensors.append(create_ogc_dummy_sensor(type, url, init))
    while True:
        for dummy in dummy_sensors:
            dummy.send()
        sleep(interval)


if __name__ == "__main__":
    main()
