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

def get_label(label): # 1 -> [1]
    return build_entry([label, 0, 0], 20)

def get_mask(byte):  # 3 -> [0, 255, 255, 255]
    if byte == 32:
        return build_entry([0] * 0 + [255] * 4, 32)
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

def get_vlan_access_check(proc_id, stage_id, matcher_id, action_id, hit):  # [1, 2, 3]
    table = []
    for h in hit:
        table.append({
            "key": [
                get_port(h)
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


def get_vlan_switch_table(proc_id, stage_id, matcher_id, action_id, mapping):  # [1, "00:00:00:00:00:00", 2]
    table = []
    for m in mapping:
        table.append({
            "key": [
                get_vlan(m[0]),
                get_mac(m[1])
            ],
            "value": [
                get_port(m[2])
            ]
        })
    return {
        "proc_id": proc_id,
        "stage_id": stage_id,
        "matcher_id": matcher_id,
        "table": table
    }


def get_mpls_push_table(proc_id, stage_id, matcher_id, action_id, push_table): # ['20.1.1.243', 1]
    table = []
    for p in push_table:
        table.append({
            "key": [
                get_ip(p[0])
            ],
            "value": [
                get_action(action_id),
                get_label(p[1])
            ]
        })
    return {
        "proc_id": proc_id,
        "stage_id": stage_id,
        "matcher_id": matcher_id,
        "table": table
}


def get_mpls_trans_table(proc_id, stage_id, matcher_id, action_id, trans_table):# [1, 2]
    table = []
    for t in trans_table:
        table.append({
            "key": [
                get_label(t[0])
            ],
            "value": [
                get_action(action_id),
                get_port(t[0]),
                get_label(t[1])
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


# Press the green button in the gutter to run the script.
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
    # router_proc_id = 1
    # router_stage_id = 1
    # router_matcher_id = 0
    # router_action_id = 4
    # dest_check_proc_id = 0
    # dest_check_stage_id = 0
    # dest_check_matcher_id = 0
    # dest_check_action_id = 7
    path = 'entry/mpls/'
    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                             [
                                 ["10.1.1.243", 32, "00:00:00:00:01:00", "00:00:00:00:00:01", 1],
                                 [  "10.1.1.3", 32, "00:00:00:00:01:01", "00:00:00:00:00:02", 2],
                                 [  "20.0.0.0",  8, "00:00:00:00:01:02", "00:00:00:00:02:00", 3]
                             ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:01:00"],
                                ["00:00:00:00:01:01"],
                                ["00:00:00:00:01:02"]
                            ])
    get_json([table1, table2], path + '1.json')

    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                             [
                                 [ "10.1.1.0", 24, "00:00:00:00:02:00", "00:00:00:00:01:01", 1],
                                 [ "20.0.0.0",  8, "00:00:00:00:02:01", "00:00:00:00:03:00", 2]
                             ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:02:00"],
                                ["00:00:00:00:02:01"]
                            ])
    get_json([table1, table2], path + '2.json')

    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                             [
                                 [ "10.1.0.0", 16, "00:00:00:00:03:00", "00:00:00:00:02:01", 1],
                                 [ "30.0.0.0",  8, "00:00:00:00:03:01", "00:00:00:00:05:00", 2],
                                 [ "20.0.0.0",  8, "00:00:00:00:03:02", "00:00:00:00:04:02", 3]
                             ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:03:00"],
                                ["00:00:00:00:03:01"],
                                ["00:00:00:00:03:02"]
                            ])
    get_json([table1, table2], path + '3.json')

    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                             [
                                 [ "20.1.0.0", 16, "00:00:00:00:04:00", "00:00:00:00:06:01", 1],
                                 [ "30.0.0.0",  8, "00:00:00:00:04:01", "00:00:00:00:05:01", 2],
                                 [ "10.0.0.0",  8, "00:00:00:00:04:02", "00:00:00:00:03:02", 3]
                             ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:04:00"],
                                ["00:00:00:00:04:01"],
                                ["00:00:00:00:04:02"]
                            ])
    get_json([table1, table2], path + '4.json')

    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                             [
                                 [ "10.0.0.0",  8, "00:00:00:00:05:00", "00:00:00:00:03:01", 1],
                                 [ "20.0.0.0",  8, "00:00:00:00:05:01", "00:00:00:00:04:01", 2]
                             ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:05:00"],
                                ["00:00:00:00:05:01"]
                            ])
    get_json([table1, table2], path + '5.json')

    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                             [
                                 [ "20.1.1.0", 24, "00:00:00:00:06:00", "00:00:00:00:07:02", 1],
                                 [ "10.0.0.0",  8, "00:00:00:00:06:01", "00:00:00:00:04:00", 2]
                             ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:06:00"],
                                ["00:00:00:00:06:01"]
                            ])
    get_json([table1, table2], path + '6.json')

    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                             [
                                 ["20.1.1.243", 32, "00:00:00:00:07:00", "00:00:00:00:00:03", 1],
                                 [  "20.1.1.3", 32, "00:00:00:00:07:01", "00:00:00:00:00:04", 2],
                                 [  "10.0.0.0",  8, "00:00:00:00:07:02", "00:00:00:00:06:00", 3]
                             ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:07:00"],
                                ["00:00:00:00:07:01"],
                                ["00:00:00:00:07:02"]
                            ])
    get_json([table1, table2], path + '7.json')


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

    mpls_ler_proc_id = 3
    mpls_ler_stage_id = 7
    mpls_ler_matcher_id = 0
    mpls_ler_action_id = 9

    mpls_lsr_proc_id = 1
    mpls_lsr_stage_id = 4
    mpls_lsr_matcher_id = 0
    mpls_lsr_action_id = 7

# without rp4 config.
#     router_proc_id = 1
#     router_stage_id = 1
#     router_matcher_id = 0
#     router_action_id = 4
#     dest_check_proc_id = 0
#     dest_check_stage_id = 0
#     dest_check_matcher_id = 0
#     dest_check_action_id = 7
#     mpls_ler_proc_id = 2
#     mpls_ler_stage_id = 3
#     mpls_ler_matcher_id = 0
#     mpls_ler_action_id = 2
#     mpls_lsr_proc_id = 1
#     mpls_lsr_stage_id = 4
#     mpls_lsr_matcher_id = 0
#     mpls_lsr_action_id = 9


    path = 'entry/mpls/new/'

    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                              [
                                  ["10.1.1.243", 32, "00:00:00:00:01:00", "00:00:00:00:00:01", 1],
                                  [  "10.1.1.3", 32, "00:00:00:00:01:01", "00:00:00:00:00:02", 2],
                                  [  "20.0.0.0",  8, "00:00:00:00:01:02", "00:00:00:00:07:02", 3]
                              ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:01:00"],
                                ["00:00:00:00:01:01"],
                                ["00:00:00:00:01:02"]
                            ])
    get_json([table1, table2], path + '1.json')

    table1 = get_router_table(router_proc_id, router_stage_id, router_matcher_id, router_action_id,
                              [
                                  ["20.1.1.243", 32, "00:00:00:00:07:00", "00:00:00:00:00:03", 1],
                                  [  "20.1.1.3", 32, "00:00:00:00:07:01", "00:00:00:00:00:04", 2],
                                  [  "10.0.0.0",  8, "00:00:00:00:07:02", "00:00:00:00:01:02", 3]
                              ])
    table2 = get_dest_check(dest_check_proc_id, dest_check_stage_id, dest_check_matcher_id, dest_check_action_id,
                            [
                                ["00:00:00:00:07:00"],
                                ["00:00:00:00:07:01"],
                                ["00:00:00:00:07:02"]
                            ])
    get_json([table1, table2], path + '7.json')

    table1 = get_mpls_push_table(mpls_ler_proc_id, mpls_ler_stage_id, mpls_ler_matcher_id, mpls_ler_action_id,
                              [
                                  ['20.1.1.243', 2],
                                  ['20.1.1.3'  , 2]
                              ])
    get_json([table1], path + '2.json')

    table1 = get_mpls_push_table(mpls_ler_proc_id, mpls_ler_stage_id, mpls_ler_matcher_id, mpls_ler_action_id,
                                 [
                                     ['10.1.1.243', 3],
                                     ['10.1.1.3'  , 3]
                                 ])
    get_json([table1], path + '6.json')

    table1 = get_mpls_trans_table(mpls_lsr_proc_id, mpls_lsr_stage_id, mpls_lsr_matcher_id, mpls_lsr_action_id,
                                 [
                                     [2, 2],
                                     [3, 1],
                                     [1, 1]
                                 ])
    get_json([table1], path + '3.json')

    table1 = get_mpls_trans_table(mpls_lsr_proc_id, mpls_lsr_stage_id, mpls_lsr_matcher_id, mpls_lsr_action_id,
                                  [
                                      [2, 1],
                                      [3, 1],
                                      [1, 1]
                                  ])
    get_json([table1], path + '4.json')

    table1 = get_mpls_trans_table(mpls_lsr_proc_id, mpls_lsr_stage_id, mpls_lsr_matcher_id, mpls_lsr_action_id,
                                  [
                                      [2, 1],
                                      [1, 1]
                                  ])
    get_json([table1], path + '5.json')


if __name__ == '__main__':
    get_simple_router()
    get_new_router()
