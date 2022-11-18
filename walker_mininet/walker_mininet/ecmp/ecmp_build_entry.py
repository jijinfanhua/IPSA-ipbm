# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import json


def build_entry(value, length):
    return {
        "val": value,
        "len": length
    }

def get_mac(m):  # "00:00:00:00:02:01" -> [1, 2, 0, 0, 0, 0]
    mac_addr = []
    for i in range(15, -1, -3):
        mac_addr.append(int(m[i:i + 2]))
    return build_entry(mac_addr, 48)


def get_ip(ip):  # "10.0.0.1" -> [1, 0, 0, 10]
    return build_entry(list(reversed([int(i) for i in ip.split('.')])), 32)

def get_const(const):
    return build_entry([const], 2)

def get_label(label): # 1 -> [1]
    return build_entry([label, 0, 0], 20)

def get_mask(byte):  # 3 -> [0, 255, 255, 255]
    if byte == 32:
        return build_entry([0] * 0 + [255] * 4, 32)
    elif byte == 26:
        return build_entry([192]   + [255] * 3, 32)
    elif byte == 28:
        return build_entry([240]   + [255] * 3, 32)
    elif byte == 24:
        return build_entry([0] * 1 + [255] * 3, 32)
    elif byte == 16:
        return build_entry([0] * 2 + [255] * 2, 32)
    elif byte == 8:
        return build_entry([0] * 3 + [255] * 1, 32)
    else:
        return build_entry([0] * 4 + [255] * 0, 32)


def get_action(id):
    return build_entry([id, 0], 16)


def get_port(id):
    return build_entry([id, 0], 9)


def get_vlan(id):
    return build_entry([id, 0], 12)


def get_L2_switch_table(proc_id, stage_id, matcher_id, action_id, mapping):  # ["00:00:00:00:00:00", 0]
    table = []
    for m in mapping:
        table.append({
            "key": [
                get_mac(m[0])
            ],
            "value": [
                get_action(action_id),
                get_port(m[1])
            ]
        })
    return {
        "proc_id": proc_id,
        "stage_id": stage_id,
        "matcher_id": matcher_id,
        "table": table
    }


def get_router_table(proc_id, stage_id, matcher_id, action_id, mapping):
    # ["10.0.0.1", 8, "00:00:00:00:00:00", "00:00:00:00:00:00", 1]
    table = []
    for m in mapping:
        table.append({
            "key": [
                get_ip(m[0])
            ],
            "mask": [
                get_mask(m[1])
            ],
            "value": [
                get_action(action_id),
                get_mac(m[2]),
                get_mac(m[3]),
                get_port(m[4])
            ]
        })
    return {
        "proc_id": proc_id,
        "stage_id": stage_id,
        "matcher_id": matcher_id,
        "table": table
    }


def get_dest_check(proc_id, stage_id, matcher_id, action_id, check):
    table = []
    for c in check:
        table.append({
            "key": [
                get_mac(c[0])
            ],
            "value": [
                get_action(action_id)
            ]
        })
    return {
        "proc_id": proc_id,
        "stage_id": stage_id,
        "matcher_id": matcher_id,
        "table": table
    }


def get_json(tables, file):
    s = {
        "entry": tables
    }
    s = json.dumps(s)
    open(file, 'w').write(s)


def get_simple_router():
    # s11 Router.
    router_proc_id = 3
    router_stage_id = 1
    router_matcher_id = 0
    router_action_id = 2

    dest_check_proc_id = 0
    dest_check_stage_id = 0
    dest_check_matcher_id = 0
    dest_check_action_id = 5

    path = 'entry/ecmp/'
    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                              [
                                  [  "10.1.1.83",  32, "00:00:00:00:01:01", "00:00:00:00:00:01", 1],
                                  [  "10.1.1.99",  32, "00:00:00:00:01:02", "00:00:00:00:00:02", 2],
                                  [  "10.1.1.115", 32, "00:00:00:00:01:03", "00:00:00:00:00:03", 3],
                                  [    "10.1.1.0", 24, "00:00:00:00:01:05", "00:00:00:00:05:01", 5]
                              ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:01:01"],
                                ["00:00:00:00:01:02"],
                                ["00:00:00:00:01:03"],
                                ["00:00:00:00:01:04"],
                                ["00:00:00:00:01:05"],
                                ["00:00:00:00:01:06"]
                            ])
    get_json([table1, table2], path + '11.json')


    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                              [
                                  [ "10.1.1.147", 32, "00:00:00:00:02:01", "00:00:00:00:00:04", 1],
                                  [   "10.1.1.0", 24, "00:00:00:00:02:03", "00:00:00:00:05:02", 3]
                              ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:02:01"],
                                ["00:00:00:00:02:02"],
                                ["00:00:00:00:02:03"],
                                ["00:00:00:00:02:04"]
                            ])
    get_json([table1, table2], path + '12.json')


    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                              [
                                  [ "10.1.1.211", 32, "00:00:00:00:03:01", "00:00:00:00:00:05", 1],
                                  [   "10.1.1.0", 24, "00:00:00:00:03:03", "00:00:00:00:05:03", 3]
                              ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:03:01"],
                                ["00:00:00:00:03:02"],
                                ["00:00:00:00:03:03"],
                                ["00:00:00:00:03:04"]
                            ])
    get_json([table1, table2], path + '13.json')

    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                              [
                                  [  "10.1.1.64", 26, "00:00:00:00:04:01", "00:00:00:00:01:04", 1],
                                  [ "10.1.1.128", 26, "00:00:00:00:04:02", "00:00:00:00:02:02", 2],
                                  [ "10.1.1.192", 26, "00:00:00:00:04:03", "00:00:00:00:03:02", 3]
                              ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:04:01"],
                                ["00:00:00:00:04:02"],
                                ["00:00:00:00:04:03"]
                            ])
    get_json([table1, table2], path + '14.json')

    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                              [
                                  [  "10.1.1.64", 26, "00:00:00:00:05:01", "00:00:00:00:01:05", 1],
                                  [ "10.1.1.128", 26, "00:00:00:00:05:02", "00:00:00:00:02:03", 2],
                                  [ "10.1.1.192", 26, "00:00:00:00:05:03", "00:00:00:00:03:03", 3]
                              ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:05:01"],
                                ["00:00:00:00:05:02"],
                                ["00:00:00:00:05:03"]
                            ])
    get_json([table1, table2], path + '15.json')

    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                              [
                                  [  "10.1.1.64", 26, "00:00:00:00:06:01", "00:00:00:00:01:06", 1],
                                  [ "10.1.1.128", 26, "00:00:00:00:06:02", "00:00:00:00:02:04", 2],
                                  [ "10.1.1.192", 26, "00:00:00:00:06:03", "00:00:00:00:03:04", 3]
                              ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:06:01"],
                                ["00:00:00:00:06:02"],
                                ["00:00:00:00:06:03"]
                            ])
    get_json([table1, table2], path + '16.json')

def get_hash_table(proc_id, stage_id, matcher_id, action_id, hash):
    table = []
    for h in hash:
        table.append({
            "key": [
                get_const(h[0])
            ],
            "value": [
                get_action(action_id),
                get_mac(h[1]),
                get_mac(h[2]),
                get_port(h[3])
            ]
        })
    return {
        "proc_id": proc_id,
        "stage_id": stage_id,
        "matcher_id": matcher_id,
        "table": table
    }


def get_egress_check(proc_id, stage_id, matcher_id, action_id, egress):
    table = []
    for e in egress:
        table.append({
            "key": [
                get_port(e[0])
            ],
            "value": [
                get_action(action_id)
            ]
        })
    return {
        "proc_id": proc_id,
        "stage_id": stage_id,
        "matcher_id": matcher_id,
        "table": table
    }


def get_new_router():
    # s11 Router.
    router_proc_id = 3
    router_stage_id = 1
    router_matcher_id = 0
    router_action_id = 2

    dest_check_proc_id = 0
    dest_check_stage_id = 0
    dest_check_matcher_id = 0
    dest_check_action_id = 5

    egress_proc_id = 4
    egress_stage_id = 4
    egress_matcher_id = 0
    egress_action_id = 9

    ecmp_proc_id = 5
    ecmp_stage_id = 5
    ecmp_matcher_id = 0
    ecmp_action_id = 10


    path = 'entry/ecmp/new/'
    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                              [
                                  [  "10.1.1.83",  32, "00:00:00:00:01:01", "00:00:00:00:00:01", 1],
                                  [  "10.1.1.99",  32, "00:00:00:00:01:02", "00:00:00:00:00:02", 2],
                                  [  "10.1.1.115", 32, "00:00:00:00:01:03", "00:00:00:00:00:03", 3],
                                  [    "10.1.1.0", 24, "00:00:00:00:01:05", "00:00:00:00:05:01", 5]
                              ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:01:01"],
                                ["00:00:00:00:01:02"],
                                ["00:00:00:00:01:03"],
                                ["00:00:00:00:01:04"],
                                ["00:00:00:00:01:05"],
                                ["00:00:00:00:01:06"]
                            ])

    table3 = get_hash_table(ecmp_proc_id, ecmp_stage_id, ecmp_matcher_id, ecmp_action_id,
                            [
                                [0, "00:00:00:00:01:04", "00:00:00:00:04:01", 4],
                                [1, "00:00:00:00:01:05", "00:00:00:00:05:01", 5],
                                [2, "00:00:00:00:01:06", "00:00:00:00:06:01", 6],
                                [3, "00:00:00:00:01:05", "00:00:00:00:05:01", 5]
                            ])
    table4 = get_egress_check(egress_proc_id, egress_stage_id, egress_matcher_id, egress_action_id,
                              [
                                  [4],
                                  [5],
                                  [6]
                              ]
                              )
    get_json([table1, table2, table3, table4], path + '11.json')


    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                              [
                                  [ "10.1.1.147", 32, "00:00:00:00:02:01", "00:00:00:00:00:04", 1],
                                  [   "10.1.1.0", 24, "00:00:00:00:02:03", "00:00:00:00:05:02", 3]
                              ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:02:01"],
                                ["00:00:00:00:02:02"],
                                ["00:00:00:00:02:03"],
                                ["00:00:00:00:02:04"]
                            ])
    table3 = get_hash_table(ecmp_proc_id, ecmp_stage_id, ecmp_matcher_id, ecmp_action_id,
                            [
                                [0, "00:00:00:00:02:03", "00:00:00:00:05:02", 3],
                                [1, "00:00:00:00:02:02", "00:00:00:00:04:02", 2],
                                [2, "00:00:00:00:02:04", "00:00:00:00:06:02", 4],
                                [3, "00:00:00:00:02:02", "00:00:00:00:04:02", 2]
                            ])
    table4 = get_egress_check(egress_proc_id, egress_stage_id, egress_matcher_id, egress_action_id,
                              [
                                  [2],
                                  [3],
                                  [4]
                              ]
                              )
    get_json([table1, table2, table3, table4], path + '12.json')


    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                              [
                                  [ "10.1.1.211", 32, "00:00:00:00:03:01", "00:00:00:00:00:05", 1],
                                  [   "10.1.1.0", 24, "00:00:00:00:03:03", "00:00:00:00:05:03", 3]
                              ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:03:01"],
                                ["00:00:00:00:03:02"],
                                ["00:00:00:00:03:03"],
                                ["00:00:00:00:03:04"]
                            ])
    table3 = get_hash_table(ecmp_proc_id, ecmp_stage_id, ecmp_matcher_id, ecmp_action_id,
                            [
                                [0, "00:00:00:00:03:03", "00:00:00:00:05:03", 3],
                                [1, "00:00:00:00:03:02", "00:00:00:00:04:03", 2],
                                [2, "00:00:00:00:03:04", "00:00:00:00:06:03", 4],
                                [3, "00:00:00:00:03:03", "00:00:00:00:05:03", 3]
                            ])
    table4 = get_egress_check(egress_proc_id, egress_stage_id, egress_matcher_id, egress_action_id,
                              [
                                  [2],
                                  [3],
                                  [4]
                              ]
                              )
    get_json([table1, table2, table3, table4], path + '13.json')



if __name__ == '__main__':
    get_simple_router()
    get_new_router()
