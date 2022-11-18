
import json

f = open("../../config/add_header.txt")

header = {}
header["previous_header_names"] = []
header["fields"] = []
header["next_headers"] = []

while True:
    line = f.readline()
    if not line:
        break
    elif line == "\n":
        continue
    line = line.strip("\n")
    if line[0] == '/':
        continue

    l = line.split()
    if l[0] == 'b':
        header["header_name"] = l[1]
        header["header_length"] = int(l[2])
        header["next_header_type_internal_offset"] = int(l[3])
        header["next_header_type_length"] = int(l[4])
    elif l[0] == 'p':
        size = len(l)
        for i in range(1, size):
            header["previous_header_names"].append(l[i])
    elif l[0] == 'f':
        field = {}
        field["field_name"] = l[1]
        field["field_length"] = int(l[2])
        field["internal_offset"] = int(l[3])
        header["fields"].append(field)
    elif l[0] == 'n':
        next_header = {}
        next_header["next_header_tag"] = l[1]
        next_header["next_header_name"] = l[2]
        header["next_headers"].append(next_header)
    elif l[0] == 't':
        header["header_tag"] = l[1]

f.close()

print(header)
print(json.dumps(header, indent=3))

filename = "../../config/add_header.json"
with open(filename, 'w') as file_obj:
    json.dump(header, file_obj, indent=3)