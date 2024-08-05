# reads hex byte strings from client

import socket
import struct

# magic trick
def twos_comp(val, bits):
  """compute the 2's complement of int value val"""
  if (val & (1 << (bits - 1))) != 0: # if sign bit is set e.g., 8bit: 128-255
      val = val - (1 << bits)        # compute negative value
  return val                         # return positive value as is

print("\nlos geht's\n\n")

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('192.168.178.20', 8282))
server.listen(5)

while True:
  client, addr = server.accept()

  # get hex string
  client_hex = client.recv(1024)
  
  laenge = len(client_hex)
  count_sets = laenge // 20
  
  print(f"\n### NEW VALUES ### ({count_sets} sets)")
  
  # split in sets
  all_hex_sets = [client_hex[i:i+20] for i in range(0, laenge, 20)]  
  for hex_set in all_hex_sets:
    print("===========================") 

    hex_item = [hex_set[i:i+8] for i in range(2, 18, 8)]
    
    # get type
    this_type = ''.join([chr(int(hex_set[0:2], 16))])
    print(f"TYPE  {this_type}")
    
    # get value
    if this_type == 'i':
      print(f"VALUE {twos_comp(int(hex_item[0],16), 32)}") 
    elif this_type == 'f':
      print(f"VALUE {struct.unpack('!f', bytes.fromhex(hex_item[0].decode()))[0]}")

    # get id
    result_string = ''.join([chr(int(hex_item[1][i:i+2], 16)) for i in range(0, len(hex_item[1]), 2)])
    print(f"ID    {result_string}")

  print("===========================") 
