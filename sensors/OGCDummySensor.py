#!/usr/bin/python3

import datetime
import json
import logging
import requests
import sys

import Sensor


class OGCDummySensor():

    def __init__(self, sensor, stream_url):
        if not isinstance(sensor, Sensor.Sensor):
            raise TypeError("Parameter \"sensor\" must be of type Sensor()")
        self.__sensor = sensor
        self.__stream = stream_url

    def send(self):
        """get next sensor value and POST it to the stream"""
        payload = dict()
        payload['result'] = self.__sensor.read()
        payload['resultTime'] = datetime.datetime.utcnow().isoformat() + "Z"
        logging.debug("payload: %s" % (json.dumps(payload)))
        r = requests.post(self.__stream, json=payload)
        if r.ok:
            logging.info("POST success (%d)" % (r.status_code))
        else:
            logging.error("POST failed (%s)" % (r.status_code))


def create_ogc_dummy_sensor(type, url):
    if type.upper().startswith("HUM"):
        s = Sensor.Humidity()
    elif type.upper().startswith("PRESS"):
        s = Sensor.Pressure()
    elif type.upper().startswith("TEMP"):
        s = Sensor.Temperature()
    else:
        raise TypeError("Invalid TYPE (humidity, pressure, temperature)")
    return OGCDummySensor(s, url)


def main():
    logging.basicConfig(level=logging.DEBUG)

    if len(sys.argv) != 3:
        print("USAGE: %s SENSOR_TYPE STREAM_URL")
        sys.exit(1)
    logging.debug("ARGV: (%s, %s)" % (sys.argv[1], sys.argv[2]))
    dummy = create_ogc_dummy_sensor(sys.argv[1], sys.argv[2])
    dummy.send()


if __name__ == "__main__":
    main()
