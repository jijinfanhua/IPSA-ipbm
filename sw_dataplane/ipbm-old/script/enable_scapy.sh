#!/usr/bin/env bash

sudo setcap cap_net_raw=eip $(realpath $(which python))
