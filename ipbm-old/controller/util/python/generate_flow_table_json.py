
import json

f = open("../../config/flow_table.txt")

flow_table_map = {}

while True:
    line = f.readline()
    if not line:
        break

    if line[0] == '#' or line[0] == "/":
        continue

    single_table_map = {}
    l = line.split()
    single_table_map["flow_table_name"] = l[0]
    single_table_map["match_type"] = l[1]
    single_table_map["mem_type"] = l[2]
    single_table_map["key_length"] = int(l[3])
    single_table_map["value_length"] = int(l[4])
    single_table_map["action_data_num"] = int(l[5])
    single_table_map["possible_entry_num"] = int(l[6])
    flow_table_map[l[0]] = single_table_map

f.close()

print(flow_table_map)
# json_string = json.dumps(flow_table_map, indent=3)
# print(json_string)

filename = "../../config/flow_table.json"
with open(filename, 'w') as file_obj:
    json.dump(flow_table_map, file_obj, indent=3)