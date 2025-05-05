'''
This script defines a Controller class that receives telemetry from a tank simulation,
logs sensor data, and sends commands to control a tank's behavior, including
searching and attacking other tanks while moving around the island.
'''

import socket
from struct import *
import datetime, time
import sys
import math
import numpy as np

from TelemetryDictionary import telemetrydirs as td
from Command import Command
from Command import Recorder
import Configuration
from Fps import Fps

FIRE = 11

def aim(values1, values2):
    angle = getAzimuth(values1[td['x']], values1[td['z']], values2[td['x']], values2[td['z']])
    bearing1 = values1[td['bearing']]
    angle2 = angle - bearing1
    return angle2

def getAzimuth(x1, z1, x2, z2):
    dx = (x2 - x1)
    dz = (z2 - z1)

    val = np.arctan2(dz, dx) * 180.0 / np.pi

    if val >= 90:
        val -= 90
    else:
        val += 270

    return val

class Controller:
    def __init__(self, tankparam):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        tankparam = int(tankparam)
        port = 4600 + tankparam
        self.server_address = ('0.0.0.0', port)
        print('Starting up on %s port %s' % self.server_address)

        self.sock.bind(self.server_address)
        self.sock.settimeout(10)

        self.length = 80
        self.unpackcode = '<Lififfffffffffffffff'

        self.recorder = Recorder()

        self.tank = tankparam
        self.mytimer = 0
        self.fps = Fps()
        self.fps.tic()

    def read(self):
        data, address = self.sock.recvfrom(self.length)
        if len(data) > 0 and len(data) == self.length:
            return unpack(self.unpackcode, data)
        return None

    def run(self):
        command = Command(Configuration.ip, 4500 + self.tank)

        moving_clockwise = True

        ts = time.time()
        st = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d-%H-%M-%S')
        f = open(f'./data/sensor.{st}.dat', 'w')

        shouldrun = True
        while shouldrun:
            try:
                tank_packets = []
                while len(tank_packets) < 5:  # Intentamos leer hasta 5 tanques (cambia si hay más)
                    try:
                        packet = self.read()
                        if packet:
                            tank_packets.append(packet)
                    except socket.timeout:
                        break  # Si no llegan más datos, seguimos

                myvalues = None
                enemy_values = []

                for packet in tank_packets:
                    if int(packet[td['number']]) == self.tank:
                        myvalues = packet
                    else:
                        enemy_values.append(packet)

                if myvalues is None:
                    continue  # Si no encontramos nuestro tanque, salteamos

                self.fps.steptoc()

                if int(myvalues[td['timer']]) < self.mytimer:
                    self.recorder.newepisode()
                    print("New Episode")
                    self.mytimer = int(myvalues[td['timer']]) - 1

                for enemy in enemy_values:
                    self.recorder.recordvalues(myvalues, enemy)

                f.write(','.join([str(myvalues[0]), str(myvalues[1]), str(myvalues[2]),
                                  str(myvalues[3]), str(myvalues[4]), str(myvalues[6])]) + '\n')
                f.flush()

                my_x = float(myvalues[td['x']])
                my_z = float(myvalues[td['z']])

                polardistance = math.sqrt(my_x**2 + my_z**2)

                # Buscar enemigo más cercano
                closest_enemy = None
                min_distance = float('inf')
                for enemy in enemy_values:
                    ex = float(enemy[td['x']])
                    ez = float(enemy[td['z']])
                    distance = math.sqrt((ex - my_x)**2 + (ez - my_z)**2)
                    if distance < min_distance:
                        min_distance = distance
                        closest_enemy = enemy

                if closest_enemy:
                    print(f"Closest enemy at distance: {min_distance}")

                print(f"Time: {myvalues[td['timer']]} Polar Distance: {polardistance}")

                turretbearing = 0.0
                if closest_enemy:
                    turretbearing = aim(myvalues, closest_enemy)

                turretdeclination = np.random.uniform(-0.4, 0.4)

                # 🚀 Movimiento más agresivo
                thrust = 12.0
                steering = np.random.uniform(-0.5, 0.5)  # Zigzag aleatorio leve

                if moving_clockwise:
                    steering += 0.5
                else:
                    steering -= 0.5

                # No caerse: si se aleja demasiado
                if polardistance > 1800:
                    moving_clockwise = not moving_clockwise

                # 🔥 Dispara SIEMPRE
                command.fire()

                command.send_command(
                    myvalues[td['timer']],
                    self.tank,
                    thrust,
                    steering,
                    turretdeclination,
                    turretbearing
                )

                self.mytimer += 1

            except socket.timeout:
                print("Episode Completed")
                break

        f.close()
        print('Everything successfully closed.')


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python Controller.py [tank_number]")
        sys.exit(1)

    controller = Controller(sys.argv[1])
    controller.run()
