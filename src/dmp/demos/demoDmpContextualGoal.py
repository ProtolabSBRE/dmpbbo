# This file is part of DmpBbo, a set of libraries and programs for the 
# black-box optimization of dynamical movement primitives.
# Copyright (C) 2014 Freek Stulp, ENSTA-ParisTech
# 
# DmpBbo is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
# 
# DmpBbo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with DmpBbo.  If not, see <http://www.gnu.org/licenses/>.


## \file demoDmpContextual.py
## \author Freek Stulp
## \brief  Visualizes results of demoDmpContextual.cpp
## 
## \ingroup Demos
## \ingroup Dmps

import numpy
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import os, sys, subprocess

lib_path = os.path.abspath('../plotting')
sys.path.append(lib_path)

from plotTrajectory import plotTrajectoryFromFile
from plotDmp import plotDmp

executable = "../../../bin/demoDmpContextualGoal"

if (not os.path.isfile(executable)):
    print("")
    print("ERROR: Executable '"+executable+"' does not exist.")
    print("Please call 'make install' in the build directory first.")
    print("")
    sys.exit(-1);

# Call the executable with the directory to which results should be written
main_directory = "/tmp/demoDmpContextualGoal"

# Test both 1-step and 2-step Dmps
for n_dmp_contextual_step in [2]:
    print("_______________________________________________________________")
    print("Demo for "+str(n_dmp_contextual_step)+"-step contextual Dmp")
    
    directory = main_directory + "/Step"+str(n_dmp_contextual_step)

    command = executable+" "+directory+" "+str(n_dmp_contextual_step)
    print(command)
    subprocess.call(command, shell=True)
    
    print("Plotting")
    
    task_parameters_demos   = numpy.loadtxt(directory+"/task_parameters_demos.txt")
    task_parameters_repros   = numpy.loadtxt(directory+"/task_parameters_repros.txt")
    n_demos = len(task_parameters_demos)
    n_repros = len(task_parameters_repros)
    
    fig = plt.figure(n_dmp_contextual_step)
    fig.suptitle(str(n_dmp_contextual_step)+"-step Contextual Dmp")
    axs = [ fig.add_subplot(131), fig.add_subplot(132), fig.add_subplot(133) ] 
    
    for i_demo in range(n_demos):
      filename = "demonstration0"+str(i_demo);
      lines = plotTrajectoryFromFile(directory+"/"+filename+".txt",axs)
      plt.setp(lines, linestyle='-',  linewidth=4, color=(0.7,0.7,1.0), label=filename)
    
    for i_repro in range(n_repros):
      filename = "reproduced0"+str(i_repro);
      lines = plotTrajectoryFromFile(directory+"/"+filename+".txt",axs)
      plt.setp(lines, linestyle='-', linewidth=1, color=(0.5,0.0,0.0), label=filename)
    
    #ax = fig.add_subplot(144)
    #inputs   = numpy.loadtxt(directory+'/inputs.txt')
    #targets  = numpy.loadtxt(directory+'/targets.txt')
    #lines = ax.plot(inputs[:,0],targets,linestyle='-',color='red')
    #ax.set_xlabel('input')
    #ax.set_ylabel('targets')
    
    #ax = fig.add_subplot(155)
    #for i_repro in range(n_repros):
    #  filename = directory+"/"+"reproduced_forcingterm0"+str(i_repro)+".txt";
    #  forcingterm = numpy.loadtxt(filename);
    #  lines = ax.plot(numpy.linspace(0,1,len(forcingterm)),forcingterm)
    #  plt.setp(lines, linestyle='-', linewidth=2, color=(0.0,0.0,0.5), label=filename)
    #ax.set_xlabel('phase')
    #ax.set_ylabel('forcing term')
      
    
    
    #plt.legend()
plt.show()
