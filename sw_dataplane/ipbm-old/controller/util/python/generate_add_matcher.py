
import json

f = open("../../config/add_matcher.txt")

matcher = {}
matcher["match_fields"] = []

while True:
    line = f.readline()
    if not line:
        break

    if line == "\n":
        continue

    l = line.strip("\n").split()
    if l[0] == 'm':
        matcher["processor_name"] = l[1]
    elif l[0] == 't':
        matcher["flow_table_name"] = l[1]
    elif l[0] == 'f':
        header_name = l[1].split('.')[0]
        field_name = l[1].split('.')[1]

        match_field = {}
        match_field["type"] = header_name
        match_field["value"] = field_name
        match_field["length"] = int(l[2])
        match_field["match_type"] = l[3]

        matcher["match_fields"].append(match_field)

f.close()

print(json.dumps(matcher, indent=3))
filename = "../../config/add_matcher.json"
with open(filename, 'w') as file_obj:
    json.dump(matcher, file_obj, indent=3)