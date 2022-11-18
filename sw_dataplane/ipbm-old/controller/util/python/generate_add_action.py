
import json

f = open("../../config/add_action.txt")

processors = []
action_map = {}
actions = []

primitive_list = []

primitive_num = 0
parameter_num = 0
primitive_idx = 0

cur_idx = 0

while True:
    line = f.readline()
    print(line)
    if not line:
        break
    if line == "\n":
        continue
    l = line.split()
    if l[0] == 'e':
        action_map["processor_name"] = l[1]
        # processors.append(l[1])
        # action_map[processors[-1]] = []
        # action_map[processors[-1]] = {}
    if l[0] == 'a':
        actions.append(l[1])

        # action_map[processors[-1]].append({})
        action_map["action_name"] = l[1]
        action_map["parameter_num"] = int(l[2])
        parameter_num = int(l[2])
        action_map["primitives"] = []
        action_map["parameter_length"] = []

        primitive_num = int(l[3])
        # primitive_list = []

        cur_idx = 0
    elif l[0] == 'p':
        primitive_map = {}
        primitive_name = l[1]
        primitive_map["primitive_name"] = l[1]

        parameters = []
        for i in range(len(l)-2):
            header_name = l[2+i].split('.')[0]
            field_name = l[2+i].split('.')[1]
            parameter_map = {}
            parameter_map["type"] = header_name
            parameter_map["value"] = field_name
            parameters.append(parameter_map)
        primitive_map["parameters"] = parameters
        action_map["primitives"].append(primitive_map)

    elif l[0] == 'l':
        for i in range(1, parameter_num + 1):
            action_map["parameter_length"].append(int(l[i]))


f.close()

print(action_map)
print(json.dumps(action_map, indent=3))

filename = "../../config/add_action.json"
with open(filename, 'w') as file_obj:
    json.dump(action_map, file_obj, indent=3)