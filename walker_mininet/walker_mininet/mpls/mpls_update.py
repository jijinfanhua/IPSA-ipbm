import os, sys, json, subprocess, re
if __name__ == '__main__':
    exe_path = '../sw_dataplane/bin'
    config_path = os.getcwd()
    topo_file = 'topology/mpls_new_topology.json'
    logs_dir  = 'logs/mpls/update/'
    with open(topo_file, 'r') as f:
        topo = json.load(f)
    switches = topo['switches']
    for switch in switches:
        if switch['auto']:
            cmd = [
                os.path.join(exe_path, 'controller'),
                str(switch['rpc_port']),
                os.path.join(config_path, switch['config']),
                os.path.join(config_path, switch['entry_table']),
                '1>' + os.path.join(config_path, logs_dir + switch['name'] + '_controller.log'),
                '2>' + os.path.join(config_path, logs_dir + switch['name'] + '_controller.log')
            ]
            cmd = ' '.join(cmd)
            print(cmd)
            os.system(cmd)
