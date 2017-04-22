import numpy as np
import matplotlib.pyplot as pp
import matplotlib
import math

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
        wnd = int(fname.split(".")[0])
        data[wnd] = dict()
        with open(fname) as f:
            for l in f.readlines():
                if "delay" in l or "Average" in l:
                    tkns  = l.split(":")
                    data[wnd][tkns[0].strip()] = tkns[1]
    return data

def get_wind_tput_delay():
    raw_data = read_data()
    data = [[],[],[]]
    for w in sorted(raw_data.keys()):
        d = raw_data[w]
        data[0].append(w)
        data[1].append(float(d["Average throughput"].split()[0]))
        data[2].append(float(d["95th percentile signal delay"].split()[0]))
    return data

def plot_tput_delay():
    setupMPPDefaults()
    wnd,tput,delay = get_wind_tput_delay()
    area = 200
    fig = pp.figure()
    pp.scatter(delay, tput, s=area, alpha=0.5)
    for i, wnd in enumerate(wnd):
        xoff = 0.98
        yoff = 1.025
        power = math.log(tput[i]/delay[i])
        print power
        if wnd in [60,80,100]:
            xoff = 1.17 + (wnd-80)/200.0
            yoff = 0.95 + wnd/10000.0
            print yoff
        pp.annotate(str(wnd), (delay[i] * xoff, tput[i] * yoff))

    pp.gca().invert_xaxis()
    pp.gca().set_xlabel("95th percentile signal delay (ms)");
    pp.gca().set_ylabel("Average throughput (Mbits/s)");
    pp.gca().spines['right'].set_visible(False)
    pp.gca().spines['top'].set_visible(False)
    pp.gca().yaxis.set_ticks_position('left')
    pp.gca().xaxis.set_ticks_position('bottom')
    pp.gca().set_xscale('log')
    pp.gca().set_xticks([70, 100, 200, 300, 400, 500, 1000, 2000, 4000, 8000])
    pp.gca().get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
    pp.gca().set_ylim([0,6])
    fig.suptitle('Throughput-delay plot', fontsize=20)
    pp.savefig("A.pdf")

def plot_score(log=False):
    setupMPPDefaults()
    wnd,tput,delay = get_wind_tput_delay()
    power = []
    fig = pp.figure()
    for i, w in enumerate(wnd):
        if log:
            power.append(math.log(tput[i]/delay[i]*1000))
        else:
            power.append(tput[i]/delay[i]*1000)
    print wnd
    print power
    pp.plot(wnd, power, 'ro')
    pp.gca().set_xlabel("Window size (static)");
    if log:
        pp.gca().set_ylabel("log(throughput(Mbit/s)/delay(s))");
    else:
        pp.gca().set_ylabel("throughput(Mbit/s)/delay(s)");
    pp.gca().spines['right'].set_visible(False)
    pp.gca().spines['top'].set_visible(False)
    pp.gca().yaxis.set_ticks_position('left')
    pp.gca().xaxis.set_ticks_position('bottom')
    pp.gca().set_xscale('log')
    pp.gca().set_xticks([1, 2, 5, 10, 20, 50, 100, 200, 500, 1000])
    pp.gca().get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
    #pp.gca().set_ylim([0,6])
    fig.suptitle('Score vs window size plot', fontsize=20)
    if log:
        pp.savefig("A-score-log.pdf")
    else:
        pp.savefig("A-score.pdf")

plot_tput_delay()
plot_score()
plot_score(log=True)
