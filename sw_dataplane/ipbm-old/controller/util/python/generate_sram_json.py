
import math
import json

sram_num = 0
sram_bit = 0

sram_idx = 0

sram_list = []

f = open("../../config/sram_config.txt")

while True:
    line = f.readline()
    if not line:
        break
    l = line.split()
    if l[0] == 'n':
        sram_num, sram_bit = int(l[1]), int(l[2])
    elif l[0] == 's':
        len_num = len(l) - 1
        if line.find('e') != -1: #fairly
            widths = []
            for i in range(len_num):
                widths.append(int(l[i+1].split(":")[0]))
            entry_num = int(math.floor(sram_bit / sum(widths)))
            width_map = {}
            for ele in widths:
                width_map[ele] = entry_num
            sram_list.append(width_map)
        else:
            width_map = {}
            for i in range(len_num):
                config = l[i+1].split(":")
                width_map[int(config[0])] = int(config[1])
            sram_list.append(width_map)

print(sram_list)

json_data = {}

json_data["sram_num"] = sram_num
json_data["sram_bit"] = sram_bit

srams = {}
for i in range(len(sram_list)):
    width_map = sram_list[i]
    config_map = {}
    for key, value in width_map.items():
        single_map = {}
        single_map["entry_width"] = key
        single_map["entry_num"] = value
        config_map[key] = single_map
    srams["sram"+str(i)] = config_map
json_data["srams"] = srams

# print(json_data)
# json_string = json.dumps(json_data, indent=3)
# print(json_string)

filename = "../../config/sram.json"
with open(filename, 'w') as file_obj:
    json.dump(json_data, file_obj, indent=3)

f.close()