import asyncio
from bleak import BleakScanner, BleakClient, BleakGATTCharacteristic

address = "24:71:89:cc:09:05"
UUID = "19b10000-e8f2-537e-4f6c-d104768a1214"
CHAR_UUID = "19b10001-e8f2-537e-4f6c-d104768a1214"


async def main(address):
    disconnected = asyncio.Event()
    print(f"Looking for devices with... UUID: {UUID}" )
    devices = []
    # Discover devices
    while len(devices) < 1:
        devices = await BleakScanner.discover(service_uuids = [UUID])
        for device in devices:
            print(f"Device found: {device.name} at address {device.address}")

    client = BleakClient(devices[0])
    def callback(char, data:bytearray):
        #  0 = Front
        #  1 = Back
        #  2 = Right
        #  3 = Left
        #  -1 = N.A.
        # print(f"{data.hex()}")
        match int(data.hex()):
            case 0:
                print("Front")
            case 1:
                print("Back")
            case 2:
                print("Right")
            case 3:
                print("Left")
            case _:
                print("N/A")
    async with client:
        print("Connecting...")
        await client.start_notify(CHAR_UUID, callback)
        print("Connection success! Printing all notifications")
        while True:
            await asyncio.sleep(1)

asyncio.run(main(address))