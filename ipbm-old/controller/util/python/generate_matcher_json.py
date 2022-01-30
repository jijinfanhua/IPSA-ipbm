
import json

f = open("../../config/matcher.txt", "r")

table_list = []

processor_list = []

match_map = {}

while True:
    line = f.readline()
    # print(line)
    if not line:
        break
    elif line[0] == '#' or line[0] == '/' or line == "\n":
        continue
    else:
        l = line.split()
        # print(l)
        if l[0] == 'm':
            processor_list.append(l[1])
            match_map[processor_list[-1]] = {}
        elif l[0] == 't':
            table_list.append(l[1])
            match_map[processor_list[-1]]["table_name"] = l[1]
            match_map[processor_list[-1]]["fields"] = []
            # match_map[table_list[-1]] = {}
        elif l[0] == 'f':
            header_name = l[1].split(".")[0]
            field_name = l[1].split(".")[1]
            match_type = l[2]
            field_map = {}
            field_map["header_name"] = header_name
            field_map["field_name"] = field_name
            field_map["match_type"] = match_type
            match_map[processor_list[-1]]["fields"].append(field_map)
            # match_map[table_list[-1]][header_name+"."+field_name] = field_map

f.close()

print(match_map)
# json_string = json.dumps(match_map, indent=3)
# print(json_string)

filename = "../../config/matcher.json"
with open(filename, 'w') as file_obj:
    json.dump(match_map, file_obj, indent=3)