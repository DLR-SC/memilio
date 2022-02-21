#ifndef IDE_H
#define IDE_H

#include "memilio/math/eigen.h"
#include "memilio/utils/time_series.h"

#include <vector>

namespace mio
{
/*TODO: add_damping-Funktion aus projekt (siehe contact_matrix.h)
    */
class IdeModel{
    public:
        IdeModel(TimeSeries<double> init, double dt_init, int N_init);
        void set_latencytime(double latency);
        void set_infectioustime(double infectious);
        TimeSeries<double> simulate(int t_max);
        void print_result(bool calculated_SEIR=false) const;
        void add_damping(double time, double R0t_time);
        TimeSeries<double> calculate_EIR();

    private:
        double Beta(double tau, double p=3.0, double q=10.0) const;
        double S_derivative(int idx) const;
        double num_integration_inner_integral(int idx) const;

        double timelatency=3.3;
        double timeinfectious=8.2;

        // vector containing one time Step per entry stored in an Eigen Vector (time, number of Susceptible at time t, R0t)
        TimeSeries<double> result; 
        TimeSeries<double> result_SEIR=TimeSeries<double>(4); 

        std::vector<Eigen::Vector2d> R0t; 
        int length_R0t=0;

        double dt;
        int k;
        int l;
        int N;
        };
}// namespace mio
#endif