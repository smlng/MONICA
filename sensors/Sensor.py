#!/usr/bin/python3

import logging
import random

from time import sleep


class Sensor():

    def __init__(self, min, max, precision, step=1):
        p10 = (10 ** int(precision))
        self.__p10 = p10
        self.__min = int(min * p10)
        self.__max = int(max * p10)
        if (step <= 0):
            self.__step = 1
        elif (step > 100):
            self.__step = 100
        else:
            self.__step = step
        self.__step = int(self.__step * p10)
        self.__last_value = random.randrange(self.__min, self.__max)
        logging.debug("- init (%d, %d, %d, %d)" %
                      (self.__min, self.__max, self.__p10, self.__step))

    def read(self):
        diff = random.randrange(-self.__step, self.__step)
        next = self.__last_value + diff
        if (next > self.__max):
            next = self.__max
        elif (next < self.__min):
            next = self.__min
        logging.debug("diff=%d, last=%d, next=%d" %
                      (diff, self.__last_value, next))
        self.__last_value = next
        return (next / self.__p10)

    def set(self, value):
        tmp = int(value) * self.__p10
        if (tmp > self.__max):
            self.__last_value = self.__max
        elif (tmp < self.__min):
            self.__last_value = self.__min
        else:
            self.__last_value = tmp
        logging.debug("set value to %s" %
                      (str(self.__last_value / self.__p10)))


class Humidity(Sensor):

    def __init__(self):
        logging.info("Create humidity sensor")
        super().__init__(0, 100, 1)


class Pressure(Sensor):

    def __init__(self):
        logging.info("Create pressure sensor")
        super().__init__(980, 1120, 1, 5)


class Temperature(Sensor):

    def __init__(self):
        logging.info("Create temperature sensor")
        super().__init__(-10, 30, 1)


class Wind(Sensor):

    def __init__(self):
        logging.info("Create temperature sensor")
        super().__init__(0, 30, 1, 0.5)


def main():
    logging.basicConfig(level=logging.ERROR)

    h = Humidity()
    p = Pressure()
    t = Temperature()
    w = Wind()

    h.set(70)
    p.set(1005)
    t.set(11)
    w.set(3)

    while True:
        print("H: {}, P: {}, T: {}, W: {}".format(h.read(), p.read(),
                                                  t.read(), w.read()))
        sleep(1)


if __name__ == "__main__":
    main()
