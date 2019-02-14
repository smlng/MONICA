#!/usr/bin/python3

import json
import os
import re
import sys

from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw


def main():
    if len(sys.argv) < 2:
        print("USAGE: {} <path/to/data>".format(sys.argv[0]))
        sys.exit(1)

    datapath = sys.argv[1]
    print("PATH: {}".format(datapath))
    files = [f for f in os.listdir(datapath) if re.match(r'.*fceaa57a-8760-5d21-bf49-744bde7e86fc_crowd_density_local\.txt', f)]

    if not len(files) > 0:
        print("ERROR: not matching files!")
        sys.exit(1)

    for file in files:
        with open(os.path.join(datapath, file), 'r') as handle:
            data = json.loads(handle.readline().strip())
            count = data["density_count"]
            name, suffix = file.split('.')
            font = ImageFont.truetype("~/Library/Fonts/FiraMono-Bold.ttf", 28)
            counttext = "Count: " + str(count)
            frame = name + '_frame.jpeg'
            if (os.path.isfile(os.path.join(datapath, frame))):
                img = Image.open(os.path.join(datapath, frame))
                draw = ImageDraw.Draw(img)
                draw.text((10, 100),counttext,(255,255,179),font=font)
                modimg = name + "_frame_count.jpeg"
                img.save(os.path.join(datapath, modimg))
                img.close()
            frame = name + '_density.jpeg'
            if (os.path.isfile(os.path.join(datapath, frame))):
                img = Image.open(os.path.join(datapath, frame))
                img = img.convert('RGB')
                draw = ImageDraw.Draw(img)
                draw.text((10, 100),counttext,(255,255,179),font=font)
                modimg = name + "_density_count.jpeg"
                img.save(os.path.join(datapath, modimg))
                img.close()

if __name__ == "__main__":
    main()
