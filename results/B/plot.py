import numpy as np
import matplotlib.pyplot as pp
import matplotlib
import math
import sys
import random

from os import listdir
from os.path import isfile, join

def setupMPPDefaults():
    pp.rcParams['figure.figsize'] = 14, 8
    pp.rcParams['font.size'] = 16
    pp.rcParams['mathtext.default'] = 'regular'
    pp.rcParams['ytick.labelsize'] = 18
    pp.rcParams['xtick.labelsize'] = 18
    pp.rcParams['legend.fontsize'] = 18
    pp.rcParams['lines.markersize'] = 12
    pp.rcParams['axes.titlesize'] = 20
    pp.rcParams['axes.labelsize'] = 20
    pp.rcParams['axes.edgecolor'] = 'grey'
    pp.rcParams['axes.linewidth'] = 3.0
    pp.rcParams['axes.grid'] = True
    pp.rcParams['grid.alpha'] = 0.4
    pp.rcParams['grid.color'] = 'grey'
    pp.rcParams['legend.frameon'] = True
    pp.rcParams['legend.framealpha'] = 0.4
    pp.rcParams['legend.numpoints'] = 1
    pp.rcParams['legend.scatterpoints'] = 1

def read_data():
    data_files = [f for f in listdir("./") if isfile(join("./", f)) and f.endswith("txt")]
    print data_files
    data = dict()
    for fname in data_files:
        wnd, ai, md, timeout = [int(x) for x in fname.split(".")[0].split("_")]

        data[(wnd, ai, md, timeout)] = dict()
        with open(fname) as f:
            for l in f.readlines():
                if "delay" in l or "Average" in l:
                    tkns  = l.split(":")
                    data[(wnd, ai, md, timeout)][tkns[0].strip()] = tkns[1]
    return data

def get_wind_tput_delay():
    raw_data = read_data()
    data = [[],[],[]]
    for wamt in sorted(raw_data.keys()):
        d = raw_data[wamt]
        data[0].append(wamt)
        data[1].append(float(d["Average throughput"].split()[0]))
        data[2].append(float(d["95th percentile signal delay"].split()[0]))
    return data

def plot_tput_delay(exp):
    setupMPPDefaults()
    wamttup,tput,delay = get_wind_tput_delay()
    area = 200
    fig = pp.figure()
    pp.scatter(delay, tput, s=area, alpha=0.5)
    for i, wamt in enumerate(wamttup):
        xoff = 1.05
        yoff = 1. + (random.random()-0.5) * 0.05
        power = math.log(tput[i]/delay[i])
        print power
        pp.annotate(str(wamt), (delay[i] * xoff, tput[i] * yoff))

    pp.gca().set_xlabel("95th percentile signal delay (ms)");
    pp.gca().set_ylabel("Average throughput (Mbits/s)");
    pp.gca().spines['right'].set_visible(False)
    pp.gca().spines['top'].set_visible(False)
    pp.gca().yaxis.set_ticks_position('left')
    pp.gca().xaxis.set_ticks_position('bottom')
    pp.gca().set_xscale('log')
    pp.gca().set_xticks([500, 1000, 2000, 4000, 8000])
    pp.gca().get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
    pp.gca().set_ylim([4,5.5])
    pp.gca().set_xlim([500,8000])
    pp.gca().invert_xaxis()
    fig.suptitle('Throughput-delay plot', fontsize=20)
    pp.savefig(exp + ".pdf")

def plot_score(exp, xlabel, log=False):
    setupMPPDefaults()
    wamt,tput,delay = get_wind_tput_delay()
    power = []
    fig = pp.figure()
    for i, w in enumerate(wamt):
        if log:
            power.append(math.log(tput[i]/delay[i]*1000))
        else:
            power.append(tput[i]/delay[i]*1000)
    print wamt
    print power
    pp.plot([x[0] for x in wamt], power, 'ro')

    for i, w in enumerate(wamt):
        xoff = 0.98
        yoff = 1.025
        pp.annotate(str(w), (w[0], power[i]))

    pp.gca().set_xlabel(xlabel);
    if log:
        pp.gca().set_ylabel("log(throughput(Mbit/s)/delay(s))");
    else:
        pp.gca().set_ylabel("throughput(Mbit/s)/delay(s)");
    pp.gca().spines['right'].set_visible(False)
    pp.gca().spines['top'].set_visible(False)
    pp.gca().yaxis.set_ticks_position('left')
    pp.gca().xaxis.set_ticks_position('bottom')
    pp.gca().set_xscale('log')
    pp.gca().set_xticks([1, 2, 5, 10, 20, 50, 100])
    pp.gca().get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
    #pp.gca().set_ylim([0,6])
    fig.suptitle('Score vs ' + xlabel + ' plot', fontsize=20)
    if log:
        pp.savefig(exp + "-score-log.pdf")
    else:
        pp.savefig(exp + "-score.pdf")

exp = sys.argv[1]
xlabel = sys.argv[2]

plot_tput_delay(exp)
plot_score(exp, xlabel)
plot_score(exp, xlabel, log=True)
