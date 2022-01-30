
import json
import math

f = open("../../config/tcam_config.txt")

tcam_num = 0
tcam_bit = 0

tcam_map = {}
cur_tcam_idx = 0

while True:
    line = f.readline()
    if not line:
        break
    l = line.split()
    if l[0] == 'n':
        tcam_num = int(l[1])
        tcam_bit = int(l[2])
    elif l[0] == 't':
        entry_length = int(l[1])
        entry_num = int(math.floor(tcam_bit / entry_length))
        data_map = {}
        data_map["entry_length"] = entry_length
        data_map["entry_num"] = entry_num
        tcam_map["tcam"+str(cur_tcam_idx)] = data_map
        cur_tcam_idx += 1

json_data = {}
json_data["tcam_num"] = tcam_num
json_data["tcam_bit"] = tcam_bit
json_data["tcams"] = tcam_map

# json_string = json.dumps(json_data, indent=3)
# print(json_string)

filename = "../../config/tcam.json"
with open(filename, 'w') as file_obj:
    json.dump(json_data, file_obj, indent=3)

f.close()