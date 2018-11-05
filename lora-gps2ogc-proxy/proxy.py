#!/bin/env python3
import logging
import os
import time
import ttn

import requests

from cayennelpp import LppFrame


# logging.basicConfig(level=logging.DEBUG)

app_id = "ttn-app.example"
app_key = "ttn-account-v2.example"
post_url = "http://example.com/locations"

if os.path.isfile("ttn.secrets"):
    with open("ttn.secrets", "r") as f:
        app_id = f.readline().strip()
        app_key = f.readline().strip()
        post_url = f.readline().strip()


def uplink_callback(msg, client):
    print("Uplink message")
    print("  FROM: ", msg.dev_id)
    print("  TIME: ", msg.metadata.time)
    frame = LppFrame.from_base64(msg.payload_raw)
    for d in frame.data:
        logging.debug(d)
    lat = frame.data[0].data[0]
    lon = frame.data[0].data[1]
    location = {
        "name": "Latest GPS tracker location",
        "description": "Continuously updated",
        "encodingType": "application/vnd.geo+json",
        "location": {
            "type": "Feature",
            "geometry": {
                "type": "Point",
                "coordinates": [
                    lon,
                    lat
                ]
            }
        }
    }
    print("  POST values: %s" % str(location))
    r = requests.post(post_url, json=location)
    print("  POST status: %d" % r.status_code)


ttncli = ttn.HandlerClient(app_id, app_key)

mqttcli = ttncli.data()
mqttcli.set_uplink_callback(uplink_callback)
mqttcli.connect()

while 1:
    time.sleep(10)
