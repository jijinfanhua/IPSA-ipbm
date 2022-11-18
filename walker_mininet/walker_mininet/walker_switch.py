import sys, os, tempfile, socket

from mininet.node import Switch
from mininet.log import info, error, debug


class WalkerSwitch(Switch):
#    next_grpc_port = 50051
    device_num = 1
    def __init__(self, name, sw_path="../sw_dataplane/bin/main", device_id=None, **kwargs):
        Switch.__init__(self, name, **kwargs)
        self.sw_path = sw_path
        self.device_id = WalkerSwitch.device_num
        self.log_file = 'walker_switch.log'
        WalkerSwitch.device_num += 1

    def start(self, controllers=None):
        # args = [self.sw_path, str(self.device_id)]
        # cmd = ' '.join(args)
        #
        # print('')
        # print('********** Switch Start **********')
        # # print('execute ' + cmd)
        #
        # # self.sendCmd(cmd)
        #
        # print("walker switch {} has been started.".format(self.name))
        # print('**********************************')
        # print('')
        pass

    def describe(self):
        print('**********************************')
        print("walker switch " + self.name)
        print("device id %d" % self.device_id)
        print('**********************************')
