/**
 * \file demoDmpTrainFromTrajectoryFile.cpp
 * \author Freek Stulp
 *
 * This file is part of DmpBbo, a set of libraries and programs for the 
 * black-box optimization of dynamical movement primitives.
 * Copyright (C) 2014 Freek Stulp, ENSTA-ParisTech
 * 
 * DmpBbo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * DmpBbo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with DmpBbo.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dmp/Dmp.hpp"
#include "dmp/Trajectory.hpp"

#include "dynamicalsystems/DynamicalSystem.hpp"
#include "dynamicalsystems/ExponentialSystem.hpp"
#include "dynamicalsystems/SigmoidSystem.hpp"
#include "dynamicalsystems/TimeSystem.hpp"
#include "dynamicalsystems/SpringDamperSystem.hpp"

#include "functionapproximators/FunctionApproximatorLWR.hpp"
#include "functionapproximators/MetaParametersLWR.hpp"
#include "functionapproximators/ModelParametersLWR.hpp"

#include <iostream>
#include <fstream>


using namespace std;
using namespace Eigen;
using namespace DmpBbo;

/** Main function
 * \param[in] n_args Number of arguments
 * \param[in] args Arguments themselves
 * \return Success of exection. 0 if successful.
 */
int main(int n_args, char** args)
{
  string input_txt_file("tests/trajectories/myTrajectoryFile.txt");
  string output_txt_file("result.txt");
  string result_directory("tests/results");
  if (n_args>1)
    input_txt_file = string(args[1]);
  if (n_args>2)
    output_txt_file = string(args[2]);
  if (n_args>3)
    result_directory = string(args[3]);
    
  
  cout << "Reading trajectory from TXT file: " << input_txt_file << endl;
  Trajectory trajectory = Trajectory::readFromFile(input_txt_file);

  double tau = trajectory.duration();
  cout << "tau : " << tau << endl;
  int n_time_steps = trajectory.length();
  VectorXd ts = trajectory.ts(); // Time steps
  int n_dims = trajectory.dim();
  cout << "n_dims is : " << n_dims << endl;
  
  // Initialize some meta parameters for training LWR function approximator
  int n_basis_functions = 20;
  int input_dim = 1;
  double intersection = 0.5;
  MetaParametersLWR* meta_parameters = new MetaParametersLWR(input_dim,n_basis_functions,intersection);      
  FunctionApproximatorLWR* fa_lwr = new FunctionApproximatorLWR(meta_parameters);  
  
  // Clone the function approximator for each dimension of the DMP
  vector<FunctionApproximator*> function_approximators(n_dims);    
  for (int dd=0; dd<n_dims; dd++)
    function_approximators[dd] = fa_lwr->clone();
  
  // Initialize the DMP
  Dmp* dmp = new Dmp(n_dims, function_approximators, Dmp::KULVICIUS_2012_JOINING);

  cout << "Training Dmp..." << endl;
  // And train it. Passing the save_directory will make sure the results are saved to file.
  bool overwrite = true;
  dmp->train(trajectory, result_directory, overwrite);
  // INTEGRATE DMP TO GET REPRODUCED TRAJECTORY
  Trajectory traj_reproduced;
  ts = VectorXd::LinSpaced(n_time_steps,0,tau); // Time steps
  dmp->analyticalSolution(ts,traj_reproduced);

  // Integrate again, but this time get more information
  MatrixXd xs_ana, xds_ana, forcing_terms_ana, fa_output_ana;
  dmp->analyticalSolution(ts,xs_ana,xds_ana,forcing_terms_ana,fa_output_ana);


  // WRITE THINGS TO FILE
  cout << "Saving newly DMP trajectories to: " << result_directory << endl;
  trajectory.saveToFile(result_directory,"original_traj.txt",overwrite);
  traj_reproduced.saveToFile(result_directory, output_txt_file,overwrite);

  MatrixXd output_ana(ts.size(),1+xs_ana.cols()+xds_ana.cols());
  output_ana << xs_ana, xds_ana, ts;

  //cout << "Writing trained Dmp to XML file: " << output_xml_file << endl;
  //std::ofstream ofs(output_xml_file);
  //boost::archive::xml_oarchive oa(ofs);
  //oa << boost::serialization::make_nvp("dmp",dmp);
  //ofs.close();
    
  delete meta_parameters;
  delete fa_lwr;
  delete dmp;

  return 0;
}
