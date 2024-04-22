from time import sleep
from SX127x.LoRa import *
from SX127x.board_config import BOARD
import paho.mqtt.publish as publish

BOARD.setup()

# Data Variables
# mq09 = None
# mq135 = None
# dht11 = None
# bmp_pressure = None
# bmp_altitude = None
# lightintensity = None
# pm = None
# water = None

def printValues():
    print(f"MQ09:\t", mq09)
    print(f"MQ135:\t", mq135)
    print(f"DHT11:\t", dht11)
    print(f"BMP Pressure:\t", bmp_pressure)
    print(f"BMP Altitude:\t", bmp_altitude)
    print(f"Light Intensity:\t", lightintensity)
    print(f"PM 2.5:\t", pm)
    print(f"Water:\t", water)
    print()

class LoRaRcvCont(LoRa):
    def __init__(self, verbose=False):
        super(LoRaRcvCont, self).__init__(verbose)
        self.set_mode(MODE.SLEEP)
        self.set_dio_mapping([0] * 6)

    def start(self):
        self.reset_ptr_rx()
        self.set_mode(MODE.RXCONT)
        while True:
            sleep(.5)
            rssi_value = self.get_rssi_value()
            status = self.get_modem_status()
            sys.stdout.flush()

    def on_rx_done(self):
        global mq09, mq135, dht11, bmp_altitude, bmp_pressure, lightintensity, pm, water
        self.clear_irq_flags(RxDone=1)
        payload = self.read_payload(nocheck=True)

        data_t = bytes(payload).decode("utf-8", 'ignore')
        parts = data_t.split(': ')
        name = parts[0].strip()
        value = float(parts[1].strip())
        print(data_t)
      #   if name == "MQ09":
      #       mq09 = value
      #   elif name == "MQ135":
      #       mq135 = value
      #   elif name == "DHT11":
      #       dht11 = value
      #   elif name == "BMPP":
      #       bmp_pressure = value
      #   elif name == "BMPA":
      #       bmp_altitude = value
      #   elif name == "Light":
      #       lightintensity = value
      #   elif name == "PM":
      #       pm = value
      #   elif name == "Water":
      #       water = value
        #printValues()
        
      #   msgs = [
	# 	{'topic': "sensors/mq09", 'payload': mq09},
	# 	{'topic': "sensors/mq135", 'payload': mq135},
	# 	("sensors/dht11", dht11, 1 ,False),
	# 	("sensors/bmp_pressure", bmp_pressure, 2, False),
	# 	("sensors/bmp_altitude", bmp_altitude, 1, False),
	# 	("sensors/lightintensity", lightintensity, 1, False),
	# 	("sensors/pm", pm, 2, False),
	# 	("sensors/water", water, 1, False)
	# 	]
		
        publish.single("sensors/mq09", data_t, hostname="mqtt.eclipseprojects.io")
        #publish.single("sensors/mq135", mq135, hostname="mqtt.eclipseprojects.io")
        self.set_mode(MODE.SLEEP)
        self.reset_ptr_rx()
        self.set_mode(MODE.RXCONT)

# Main Program Starts Here
lora = LoRaRcvCont(verbose=False)
lora.set_mode(MODE.STDBY)

# Medium Range Defaults after init are 434.0MHz, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on 13 dBm
lora.set_pa_config(pa_select=1)

try:
    print("Start-->Listening. . . . \n")
    lora.start()

except KeyboardInterrupt:
    sys.stdout.flush()
    print("")
    sys.stderr.write("KeyboardInterrupt\n")
finally:
    sys.stdout.flush()
    print("")
    lora.set_mode(MODE.SLEEP)
    BOARD.teardown()
