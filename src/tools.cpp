#include <iostream>
#include "tools.h"

using Eigen::VectorXd;
using Eigen::MatrixXd;
using std::vector;

Tools::Tools() {}

Tools::~Tools() {}

VectorXd Tools::CalculateRMSE(const vector<VectorXd> &estimations,
                              const vector<VectorXd> &ground_truth) {
    /*
    VectorXd rmse(4);
        rmse << 0,0,0,0;

        // check the validity of the following inputs:
        //  * the estimation vector size should not be zero
        //  * the estimation vector size should equal ground truth vector size
        if(estimations.size() != ground_truth.size()
                || estimations.size() == 0){
            cout << "Invalid estimation or ground_truth data" << endl;
            return rmse;
        }

        //accumulate squared residuals
        for(unsigned int i=0; i < estimations.size(); ++i){

            VectorXd residual = estimations[i] - ground_truth[i];

            //coefficient-wise multiplication
            residual = residual.array()*residual.array();
            rmse += residual;
        }

        //calculate the mean
        rmse = rmse/estimations.size();

        //calculate the squared root
        rmse = rmse.array().sqrt();

        //return the result
        return rmse;
        */
    float t = estimations.size(); // Current timestep index

    // check the validity of the inputs
    if( t == 0 )
      cout << "Error in CalculateRMSE:  estimations.size() = 0" << endl;
    if( t != ground_truth.size() )
      cout << "Error in CalculateRMSE: sizes of estimation and ground truth do not match" << endl;

    // Rather than recomputing from the entire estimations array,
    // I store the running error from the last timestep persistently.
    // This ensures that CalculateRMSE() remains O(1) instead of O(N)
    // at the Nth timestep.
    //
    // for(int i=0; i < estimations.size(); i++)
    // {

    // Recover the running sum of the error, rather than the running
    // mean, so we can add the current residual
    mse = mse*(t-1);

    // Add the current residual using a Kahan sum to minimize floating-point
    // rounding error.
    residual = estimations[t-1] - ground_truth[t-1];
    residual = residual.array()*residual.array();
    residual += kahanerror;
    mse += residual;
    kahanerror = residual - ( mse - lastmse );
    lastmse = mse;

    // }

    // Calculate the new mean
    mse = mse/estimations.size();

    // Calculate the RMSE
    rmse = mse.array().sqrt();

    // cout << estimations.size() << endl;
    if( rmse(0) > .09 ||
        rmse(1) > .10 ||
        rmse(2) > .40 ||
        rmse(3) > .30 )
      cout << "Warning at timestep " << t << ":  rmse = "
           << rmse(0) << "  " << rmse(1) << "  "
           << rmse(2) << "  " << rmse(3) << endl
           << " currently exceeds tolerances of "
           << ".09, .10, .40, .30" << endl;

    return rmse;
}
