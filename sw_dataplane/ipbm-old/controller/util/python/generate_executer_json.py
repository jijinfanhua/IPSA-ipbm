
import json

f = open("../../config/executer.txt")

processors = []
action_map = {}
actions = []

primitive_list = []

primitive_num = 0
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
        processors.append(l[1])
        action_map[processors[-1]] = []
        # action_map[processors[-1]] = {}
    if l[0] == 'a':
        actions.append(l[1])

        action_map[processors[-1]].append({})
        action_map[processors[-1]][-1]["action_name"] = l[1]
        action_map[processors[-1]][-1]["parameter_num"] = int(l[2])
        action_map[processors[-1]][-1]["primitives"] = []
        # action_map[processors[-1]][actions[-1]] = {}
        #
        # action_map[processors[-1]][actions[-1]]["action_name"] = l[1]
        # action_map[processors[-1]][actions[-1]]["parameter_num"] = int(l[2])
        # action_map[tables[-1]][actions[-1]]["primitives"] = {}

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
        action_map[processors[-1]][-1]["primitives"].append(primitive_map)
        # primitive_list.append(primitive_map)
        # primitive_idx += 1
        # if primitive_idx == primitive_num:
        #     action_map[processors[-1]][actions[-1]]["primitives"] = primitive_list
        #     primitive_list = []
        #     primitive_idx = 0


        # idx = "primitive"+str(cur_idx)
        # cur_idx += 1
        # action_map[tables[-1]][actions[-1]]["primitives"][idx] = {}
        # action_map[tables[-1]][actions[-1]]["primitives"][idx]["primitive_name"] = primitive_name
        # action_map[tables[-1]][actions[-1]]["primitives"][idx]["parameters"] = {}
        # for i in range(len(l)-2):
        #     primitive_parameter_map = {}
        #     header_name = l[2+i].split('.')[0]
        #     field_name = l[2+i].split('.')[1]
        #     primitive_parameter_map["type"] = header_name
        #     primitive_parameter_map["value"] = field_name
        #
        #     action_map[tables[-1]][actions[-1]]["primitives"][idx]["parameters"]["parameter"+str(i)] = primitive_parameter_map

            # action_map[tables[-1]][actions[-1]]["primitives"][idx][] = {}
            # action_map[tables[-1]][actions[-1]]["primitives"][idx]
            # action_map[tables[-1]][actions[-1]]["primitives"]["primitive"+str(i)][l[2+i]] = primitive_parameter_map

f.close()

print(action_map)
print(json.dumps(action_map, indent=3))

filename = "../../config/executer.json"
with open(filename, 'w') as file_obj:
    json.dump(action_map, file_obj, indent=3)