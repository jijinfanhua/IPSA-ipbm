import os, sys, json, subprocess, re
from time import sleep
if __name__ == '__main__':
    exe_path = '../sw_dataplane/bin'
    absolute_path = ''
    topo_file = absolute_path + 'topology/ecmp_topology.json'
    logs_dir = absolute_path + 'logs/ecmp/'
    with open(topo_file, 'r') as f:
        topo = json.load(f)
    switches = topo['switches']
    processes = []
    for switch in switches:
        if switch['auto']:
            cmd = [
                os.path.join(exe_path, 'main'),
                absolute_path + switch['interface_file'],
                str(switch['rpc_port']),
                # '1>/dev/null',
                # '2>/dev/null'
                # (('1>' + logs_dir + switch['name'] + '_server.log') if switch['output'] == 'log' else ''),
                # (('2>' + logs_dir + switch['name'] + '_server.log') if switch['output'] == 'log' else '')
            ]
            cmd = ' '.join(cmd)
            print(cmd)
            processes.append(subprocess.Popen(cmd, shell=True))
            sleep(0.5)
            # cmd = [
            #     os.path.join(exe_path, 'controller'),
            #     str(switch['rpc_port']),
            #     os.path.join(conf_path, switch['config']),
            #     os.path.join(conf_path, switch['entry_table']),
            #     '1>' + logs_dir + switch['name'] + '_controller.log',
            #     '2>' + logs_dir + switch['name'] + '_controller.log'
            # ]
            # cmd = ' '.join(cmd)
            # print(cmd)
            # os.system(cmd)

    print(processes)

    while True:
        cmd = input('You can input everything, but I have no response but let you input again.')
