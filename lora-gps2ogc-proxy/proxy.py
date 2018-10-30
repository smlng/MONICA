#!/bin/env python3


import base64
import logging
import os
import time
import ttn

from cayennelpp import LppFrame


logging.basicConfig(level=logging.DEBUG)

app_id = "ttn-app.example"
app_key = "ttn-account-v2.example"

if os.path.isfile("ttn.secrets"):
    with open("ttn.secrets", "r") as f:
        app_id = f.readline().strip()
        app_key = f.readline().strip()

def uplink_callback(msg, client):
    print("Uplink message")
    print("  FROM: ", msg.dev_id)
    print("  TIME: ", msg.metadata.time)
    frame = LppFrame.from_base64(msg.payload_raw)
    for d in frame.data():
        print(d)

ttncli = ttn.HandlerClient(app_id, app_key)

mqttcli = ttncli.data()
mqttcli.set_uplink_callback(uplink_callback)
mqttcli.connect()

while 1:
    time.sleep(10)
