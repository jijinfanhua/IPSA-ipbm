import os, sys, json, subprocess, re
from time import sleep
if __name__ == '__main__':
    exe_path = '../sw_dataplane/bin'
    topo_file = 'topology/mpls_topology.json'
    config_path = os.getcwd()
    logs_dir  = 'logs/mpls/'
    with open(topo_file, 'r') as f:
        topo = json.load(f)
    switches = topo['switches']
    processes = []
    for switch in switches:
        if switch['auto']:
            cmd = [
                os.path.join(exe_path, 'main'),
                os.path.join(config_path, switch['interface_file']),
                str(switch['rpc_port']),
                # '1>/dev/null',
                # '2>/dev/null'
#                 (('1>' + os.path.join(os.getcwd(), logs_dir + switch['name'] + '_server.log')) if switch['output'] == 'log' else ''),
#                 (('2>' + os.path.join(os.getcwd(), logs_dir + switch['name'] + '_server.log')) if switch['output'] == 'log' else '')
            ]
            cmd = ' '.join(cmd)
            print(cmd)
            processes.append(subprocess.Popen(cmd, shell=True))
            sleep(0.5)

    print(processes)

    while True:
        cmd = input('You can input everything, but I have no response but let you input again.')
