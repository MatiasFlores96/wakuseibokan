# EnemyBot mejorado y más activo
import socket
from struct import *
import sys
import math
import random
import time

from TelemetryDictionary import telemetrydirs as td
from Command import Command
import Configuration

class EnemyBot:
    def __init__(self, tank_number):
        self.tank = int(tank_number)
        self.port = 4600 + self.tank
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('0.0.0.0', self.port))
        self.sock.settimeout(5)
        self.length = 80
        self.unpackcode = '<Lififfffffffffffffff'
        self.command = Command(Configuration.ip, 4500 + self.tank)

    def read(self):
        try:
            data, _ = self.sock.recvfrom(self.length)
            if len(data) == self.length:
                return unpack(self.unpackcode, data)
        except:
            pass
        return None

    def run(self):
        print(f"EnemyBot {self.tank} ready.")

        while True:
            packets = []
            mypacket = None

            for _ in range(10):
                packet = self.read()
                if packet:
                    if int(packet[td['number']]) == self.tank:
                        mypacket = packet
                    else:
                        packets.append(packet)

            if not mypacket:
                continue

            my_x, my_z = float(mypacket[td['x']]), float(mypacket[td['z']])
            polardist = math.sqrt(my_x**2 + my_z**2)

            if packets:
                closest_enemy = min(packets, key=lambda p: math.hypot(float(p[td['x']]) - my_x, float(p[td['z']]) - my_z))
                dx = float(closest_enemy[td['x']]) - my_x
                dz = float(closest_enemy[td['z']]) - my_z
                distance = math.hypot(dx, dz)
                steer = 1.0 if dx > 0 else -1.0
                turretbearing = math.degrees(math.atan2(dz, dx))
            else:
                steer = random.choice([-1.0, 0.0, 1.0])
                distance = 9999
                turretbearing = random.uniform(-45, 45)

            thrust = 10.0
            turretdecl = random.uniform(-0.2, 0.2)

            if polardist > 1800:
                steer = -steer  # vuelve al centro si está muy lejos

            # Dispara más seguido si el enemigo está cerca
            if distance < 400 or random.random() < 0.3:
                self.command.fire()

            self.command.send_command(mypacket[td['timer']], self.tank,
                                      thrust, steer, turretdecl, turretbearing)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python EnemyBot.py [tank_number]")
        sys.exit(1)

    bot = EnemyBot(sys.argv[1])
    bot.run()
