#!/usr/bin/env python3
import logging
import os
import time
import json

import requests
import ttn
from cayennelpp import LppFrame


app_id = "ttn-app.example"
app_key = "ttn-account-v2.example"
dev_urls = {"example": {"id": 12345, "url": "http://example.com/locations"}}

f_secrets = "ttn.secrets"
f_tracker = "tracker.url"
f_logging = "lora-proxy.log"


def uplink_callback(msg, client):
    print("Uplink message")
    print("  FROM: ", msg.dev_id)
    print("  TIME: ", msg.metadata.time)
    print("   RAW: ", msg.payload_raw)
    with open(f_logging, 'a') as fl:
        print(str(msg), file = fl)
    frame = LppFrame.from_base64(msg.payload_raw)
    for d in frame.data:
        print("  DATA:", d)
    lat = frame.data[0].value[0]
    lon = frame.data[0].value[1]
    location = {
        "name": msg.dev_id,
        "description": "Continuously updated GPS location of tracker device",
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
    print("  POST values: ", str(location))
    if msg.dev_id in dev_urls:
        print("  POST URL: ", dev_urls[msg.dev_id]['url'])
        r = requests.post(dev_urls[msg.dev_id]['url'], json=location)
        print("  POST status: %d" % r.status_code)
    else:
        print("  invalid device: ", msg.dev_id)


def main():
    if os.path.isfile(f_secrets):
        with open(f_secrets, "r") as f:
            secrets = json.loads(f.read())
            app_id = secrets['app_id']
            app_key = secrets['app_key']

    print("APP ID: ", app_id)
    print("APP KEY:", app_key)

    if os.path.isfile(f_tracker):
        with open(f_tracker, "r") as f:
            global dev_urls
            dev_urls = json.loads(f.read())

    print("URLS: ", json.dumps(dev_urls, indent=2))
    with open(f_logging, 'a') as fl:
        print(json.dumps(dev_urls), file = fl)

    ttncli = ttn.HandlerClient(app_id, app_key)

    mqttcli = ttncli.data()
    mqttcli.set_uplink_callback(uplink_callback)
    mqttcli.connect()

    while 1:
        time.sleep(10)


if __name__ == "__main__":
    main()
