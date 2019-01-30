#!/usr/bin/python3

import logging
import os
import sys

from OGCDummySensor import create_ogc_dummy_sensor


def main():
    logging.basicConfig(level=logging.DEBUG)
    if len(sys.argv) < 2:
        print("USAGE: {} CSV_FILE".format(sys.argv[0]))
        sys.exit(1)
    if not os.path.isfile(sys.argv[1]):
        print("Invalid CSV_FILE")
        sys.exit(2)

    dummy_sensors = list()
    with open(sys.argv[1]) as csv:
        streams = csv.readlines()
        for s in streams:
            sensor, url = s.strip().split(";")
            dummy_sensors.append(create_ogc_dummy_sensor(sensor, url))
    for dummy in dummy_sensors:
        dummy.send()


if __name__ == "__main__":
    main()
