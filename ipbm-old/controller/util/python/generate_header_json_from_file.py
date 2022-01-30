import json
from basic_class import *

# fp = open("../config/header_pure.txt", "r")
fp = open("../../config/header_pure.txt", "r")

header_list = []

while True:
    line = fp.readline()
    # print(line)
    if line == "*":
        break
    elif line == "":
        continue
    else:
        l = line.split()
        if line[0] == 'h':
            header = Header(l[1], int(l[2]), int(l[3]))
            header_list.append(header)
        elif line[0] == 'f':
            field = Field(l[1], int(l[2]), int(l[3]))
            header_list[-1].addField(field)
        elif line[0] == 'n':
            if int(l[2]) == 0:
                continue
            header_list[-1].next_header_type_internal_offset = int(l[1])
            header_list[-1].next_header_type_length = int(l[2])
        elif line[0] == 'm':
            if l[2] == "0":
                continue
            next_header = NextHeader(l[1], l[2])
            header_list[-1].addNextHeader(next_header)

json_data = {}
for header in header_list:
    fields = header.fields
    next_headers = header.next_headers
    field_dict = {}
    next_header_dict = {}
    for field in fields:
        field_dict[field.field_name] = {"field_name": field.field_name,
                                        "field_length": field.field_length,
                                        "field_internal_offset": field.field_internal_offset}
    for next_header in next_headers:
        next_header_dict[next_header.header_tag] = {"header_tag": next_header.header_tag,
                                                    "header_name": next_header.header_name}

    json_data[header.header_name] = {"header_name": header.header_name,
                                     "header_length": header.header_length,
                                     "next_header_type_internal_offset": header.next_header_type_internal_offset,
                                     "next_header_type_length": header.next_header_type_length,
                                     "field_num": header.field_num,
                                     "fields": field_dict,
                                     "next_headers": next_header_dict}
filename = "../../config/header.json"
with open(filename, 'w') as file_obj:
    json.dump(json_data, file_obj, indent=3)
# json_string = json.dumps(json_data, indent=3)
# print(json_string)

    # print('Header: {}'.format(header.header_name))
    # print('\theader length: {}'.format(header.header_length))
    # print('\tfield num: {}'.format(header.field_num))
    # if(header.next_header_type_length != 0):
    #     print('\tnext_header_type_internal_offset: {}'.format(header.next_header_type_internal_offset))
    #     print('\tnext_header_type_length: {}'.format(header.next_header_type_length))
    # print('\tFields:')
    # for field in header.fields:
    #     print('\t\t{} {} {}'.format(field.field_name, field.field_length, field.field_internal_offset))
    # print('\tNext Headers:')
    # for next_header in header.next_headers:
    #     print('\t\t{} {}'.format(next_header.header_tag, next_header.header_name))

