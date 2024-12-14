#include "ode_ui10/model.h"
#include "ode_ui10/infection_state.h"
#include "ode_ui10/parameters.h"
#include "memilio/compartments/simulation.h"
#include "memilio/utils/logging.h"

#include <map>



std::vector<int> transmission(int variant, int immunity_old)
{   
    std::vector<int> out(2);
    std::vector<int> immunity(66);
    std::iota(immunity.begin(), immunity.end(), 0);

    std::vector<int> var = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                            101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
                            202, 203, 204, 205, 206, 207, 208, 209, 210,
                            303, 304, 305, 306, 307, 308, 309, 310,
                            404, 405, 406, 407, 408, 409, 410,
                            505, 506, 507, 508, 509, 510, 
                            606, 607, 608, 609, 610,
                            707, 708, 709, 710,
                            808, 809, 810,
                            909, 910,
                            1010};
    
    std::map<int, int> m;
    assert(immunity.size() == var.size());
    for (size_t i = 0; i < immunity.size(); ++i)
    {
        m[immunity[i]] = var[i];
    }
    std::map<int, int> m2;
    for (size_t i = 0; i < var.size(); ++i)
    {
        m2[var[i]] = immunity[i];
    }

    int var1 = m[immunity_old]/100;
    int var2 = m[immunity_old] % 100;

    int scale = 1;
    if (var2 >= variant)
    {
        if(var1 >= variant)
        {
            scale = 100;
        } else
        {
            scale = 2;
        }
    }

    if (var1 < variant) var1 = variant;
    if (var1 > var2) std::swap(var1, var2);

    out[0] = m2[100 * var1 + var2];
    out[1] = scale;
    
    return out;
}


int main()
{
    mio::set_log_level(mio::LogLevel::debug);

    double t0   = 0;
    double tmax = 1000;
    double dt   = 0.1;

    mio::log_info("Simulating UI; t={} ... {} with dt = {}.", t0, tmax, dt);

    double cont_freq = 10;

    //double nb_total_t0 = 10000;//  nb_inf_t0 = 100;
    const size_t num_groups = 66;

    mio::oui10::Model<double> model(num_groups);
    double fact = 1.0 / num_groups;

    auto& params = model.parameters;

    printf("%i\n%i\n", transmission(3, 30)[0], transmission(3, 22)[1]);

    for (auto i = mio::AgeGroup(0); i < mio::AgeGroup(num_groups); i++) {
        model.populations[{i, mio::oui10::InfectionState::InfectedV1}]  = 0;//fact * nb_inf_t0;
        model.populations[{i, mio::oui10::InfectionState::InfectedV2}]  = 0;//fact * nb_inf_t0;
        model.populations[{i, mio::oui10::InfectionState::InfectedV3}]  = 0;//fact * nb_inf_t0;
        model.populations[{i, mio::oui10::InfectionState::InfectedV4}]  = 0;
        model.populations[{i, mio::oui10::InfectionState::InfectedV5}]  = 0;
        model.populations[{i, mio::oui10::InfectionState::InfectedV6}]  = 0;
        model.populations[{i, mio::oui10::InfectionState::InfectedV7}]  = 0;
        model.populations[{i, mio::oui10::InfectionState::InfectedV8}]  = 0;
        model.populations[{i, mio::oui10::InfectionState::InfectedV9}]  = 0;
        model.populations[{i, mio::oui10::InfectionState::InfectedV10}]  = 0;
        
        //model.populations.set_difference_from_group_total<mio::AgeGroup>({i, mio::oui::InfectionState::Susceptible},
        //                                                                 fact * (nb_total_t0 - 1000));

        ScalarType base = 1.0;

        model.parameters.get<mio::oui10::TimeInfectedV1<double>>()[i]                     = 7 * std::pow(base, 0);
        model.parameters.get<mio::oui10::TimeInfectedV2<double>>()[i]                     = 7 * std::pow(base, 1);
        model.parameters.get<mio::oui10::TimeInfectedV3<double>>()[i]                     = 7 * std::pow(base, 2);
        model.parameters.get<mio::oui10::TimeInfectedV4<double>>()[i]                     = 7 * std::pow(base, 3);
        model.parameters.get<mio::oui10::TimeInfectedV5<double>>()[i]                     = 7 * std::pow(base, 4);
        model.parameters.get<mio::oui10::TimeInfectedV6<double>>()[i]                     = 7 * std::pow(base, 5);
        model.parameters.get<mio::oui10::TimeInfectedV7<double>>()[i]                     = 7 * std::pow(base, 6);
        model.parameters.get<mio::oui10::TimeInfectedV8<double>>()[i]                     = 7 * std::pow(base, 7);
        model.parameters.get<mio::oui10::TimeInfectedV9<double>>()[i]                     = 7 * std::pow(base, 8);
        model.parameters.get<mio::oui10::TimeInfectedV10<double>>()[i]                     = 7 * std::pow(base, 9);
        model.parameters.get<mio::oui10::TransmissionProbabilityOnContactV1<double>>()[i] = 0.2;
        model.parameters.get<mio::oui10::TransmissionProbabilityOnContactV2<double>>()[i] = 0.2;
        model.parameters.get<mio::oui10::TransmissionProbabilityOnContactV3<double>>()[i] = 0.2;
        model.parameters.get<mio::oui10::TransmissionProbabilityOnContactV4<double>>()[i] = 0.2;
        model.parameters.get<mio::oui10::TransmissionProbabilityOnContactV5<double>>()[i] = 0.2;
        model.parameters.get<mio::oui10::TransmissionProbabilityOnContactV6<double>>()[i] = 0.2;
        model.parameters.get<mio::oui10::TransmissionProbabilityOnContactV7<double>>()[i] = 0.2;
        model.parameters.get<mio::oui10::TransmissionProbabilityOnContactV8<double>>()[i] = 0.2;
        model.parameters.get<mio::oui10::TransmissionProbabilityOnContactV9<double>>()[i] = 0.2;
        model.parameters.get<mio::oui10::TransmissionProbabilityOnContactV10<double>>()[i] = 0.2;
    }

    model.populations[{mio::AgeGroup(0), mio::oui10::InfectionState::Susceptible}] = 9900;
    model.populations[{mio::AgeGroup(0), mio::oui10::InfectionState::InfectedV1}]  = 100;

    mio::ContactMatrixGroup& contact_matrix = params.get<mio::oui10::ContactPatterns<double>>();
    contact_matrix[0] = mio::ContactMatrix(Eigen::MatrixXd::Constant(num_groups, num_groups, fact * cont_freq));
    //contact_matrix.add_damping(Eigen::MatrixXd::Constant(num_groups, num_groups, 0.7), mio::SimulationTime(30.));

    Eigen::MatrixXd rec_1 = Eigen::MatrixXd::Constant(num_groups, num_groups, 0.0);
    Eigen::MatrixXd rec_2 = Eigen::MatrixXd::Constant(num_groups, num_groups, 0.0);
    Eigen::MatrixXd rec_3 = Eigen::MatrixXd::Constant(num_groups, num_groups, 0.0);
    Eigen::MatrixXd rec_4 = Eigen::MatrixXd::Constant(num_groups, num_groups, 0.0);
    Eigen::MatrixXd rec_5 = Eigen::MatrixXd::Constant(num_groups, num_groups, 0.0);
    Eigen::MatrixXd rec_6 = Eigen::MatrixXd::Constant(num_groups, num_groups, 0.0);
    Eigen::MatrixXd rec_7 = Eigen::MatrixXd::Constant(num_groups, num_groups, 0.0);
    Eigen::MatrixXd rec_8 = Eigen::MatrixXd::Constant(num_groups, num_groups, 0.0);
    Eigen::MatrixXd rec_9 = Eigen::MatrixXd::Constant(num_groups, num_groups, 0.0);
    Eigen::MatrixXd rec_10 = Eigen::MatrixXd::Constant(num_groups, num_groups, 0.0);
    for (auto i = 0; i < num_groups; ++i)
    {
        rec_1(transmission(1, i)[0], i) = transmission(1, i)[1];
        rec_2(transmission(2, i)[0], i) = transmission(2, i)[1];
        rec_3(transmission(3, i)[0], i) = transmission(3, i)[1];
        rec_4(transmission(4, i)[0], i) = transmission(4, i)[1];
        rec_5(transmission(5, i)[0], i) = transmission(5, i)[1];
        rec_6(transmission(6, i)[0], i) = transmission(6, i)[1];
        rec_7(transmission(7, i)[0], i) = transmission(7, i)[1];
        rec_8(transmission(8, i)[0], i) = transmission(8, i)[1];
        rec_9(transmission(9, i)[0], i) = transmission(9, i)[1];
        rec_10(transmission(10, i)[0], i) = transmission(10, i)[1];
    }


    mio::ContactMatrixGroup& recovery_matrix_v1 = params.get<mio::oui10::RecoveryPatternsV1<double>>();
    recovery_matrix_v1[0] = mio::ContactMatrix(rec_1);
    mio::ContactMatrixGroup& recovery_matrix_v2 = params.get<mio::oui10::RecoveryPatternsV2<double>>();
    recovery_matrix_v2[0] = mio::ContactMatrix(rec_2);
    mio::ContactMatrixGroup& recovery_matrix_v3 = params.get<mio::oui10::RecoveryPatternsV3<double>>();
    recovery_matrix_v3[0] = mio::ContactMatrix(rec_3);
    mio::ContactMatrixGroup& recovery_matrix_v4 = params.get<mio::oui10::RecoveryPatternsV4<double>>();
    recovery_matrix_v4[0] = mio::ContactMatrix(rec_4);
    mio::ContactMatrixGroup& recovery_matrix_v5 = params.get<mio::oui10::RecoveryPatternsV5<double>>();
    recovery_matrix_v5[0] = mio::ContactMatrix(rec_5);
    mio::ContactMatrixGroup& recovery_matrix_v6 = params.get<mio::oui10::RecoveryPatternsV6<double>>();
    recovery_matrix_v6[0] = mio::ContactMatrix(rec_6);
    mio::ContactMatrixGroup& recovery_matrix_v7 = params.get<mio::oui10::RecoveryPatternsV7<double>>();
    recovery_matrix_v7[0] = mio::ContactMatrix(rec_7);
    mio::ContactMatrixGroup& recovery_matrix_v8 = params.get<mio::oui10::RecoveryPatternsV8<double>>();
    recovery_matrix_v8[0] = mio::ContactMatrix(rec_8);
    mio::ContactMatrixGroup& recovery_matrix_v9 = params.get<mio::oui10::RecoveryPatternsV9<double>>();
    recovery_matrix_v9[0] = mio::ContactMatrix(rec_9);
    mio::ContactMatrixGroup& recovery_matrix_v10 = params.get<mio::oui10::RecoveryPatternsV10<double>>();
    recovery_matrix_v10[0] = mio::ContactMatrix(rec_10);

    //model.apply_constraints();

    auto ui = simulate(t0, tmax, dt, model);

    std::ofstream fileout;
    fileout.open("filename.txt");

    ui.print_table({"Susceptible_0", "InfectedV1_0", "InfectedV2_0", "InfectedV3_0", "InfectedV4_0", "InfectedV5_0", "InfectedV6_0", "InfectedV7_0", "InfectedV8_0", "InfectedV9_0", "InfectedV10_0",
                    "Susceptible_1", "InfectedV1_1", "InfectedV2_1", "InfectedV3_1", "InfectedV4_1", "InfectedV5_1", "InfectedV6_1", "InfectedV7_1", "InfectedV8_1", "InfectedV9_1", "InfectedV10_1", 
                    "Susceptible_2", "InfectedV1_2", "InfectedV2_2", "InfectedV3_2", "InfectedV4_2", "InfectedV5_2", "InfectedV6_2", "InfectedV7_2", "InfectedV8_2", "InfectedV9_2", "InfectedV10_2", 
                    "Susceptible_3", "InfectedV1_3", "InfectedV2_3", "InfectedV3_3", "InfectedV4_3", "InfectedV5_3", "InfectedV6_3", "InfectedV7_3", "InfectedV8_3", "InfectedV9_3", "InfectedV10_3", 
                    "Susceptible_4", "InfectedV1_4", "InfectedV2_4", "InfectedV3_4", "InfectedV4_4", "InfectedV5_4", "InfectedV6_4", "InfectedV7_4", "InfectedV8_4", "InfectedV9_4", "InfectedV10_4", 
                    "Susceptible_5", "InfectedV1_5", "InfectedV2_5", "InfectedV3_5", "InfectedV4_5", "InfectedV5_5", "InfectedV6_5", "InfectedV7_5", "InfectedV8_5", "InfectedV9_5", "InfectedV10_5", 
                    "Susceptible_6", "InfectedV1_6", "InfectedV2_6", "InfectedV3_6", "InfectedV4_6", "InfectedV5_6", "InfectedV6_6", "InfectedV7_6", "InfectedV8_6", "InfectedV9_6", "InfectedV10_6", 
                    "Susceptible_7", "InfectedV1_7", "InfectedV2_7", "InfectedV3_7", "InfectedV4_7", "InfectedV5_7", "InfectedV6_7", "InfectedV7_7", "InfectedV8_7", "InfectedV9_7", "InfectedV10_7", 
                    "Susceptible_8", "InfectedV1_8", "InfectedV2_8", "InfectedV3_8", "InfectedV4_8", "InfectedV5_8", "InfectedV6_8", "InfectedV7_8", "InfectedV8_8", "InfectedV9_8", "InfectedV10_8", 
                    "Susceptible_9", "InfectedV1_9", "InfectedV2_9", "InfectedV3_9", "InfectedV4_9", "InfectedV5_9", "InfectedV6_9", "InfectedV7_9", "InfectedV8_9", "InfectedV9_9", "InfectedV10_9", 
                    "Susceptible_10", "InfectedV1_10", "InfectedV2_10", "InfectedV3_10", "InfectedV4_10", "InfectedV5_10", "InfectedV6_10", "InfectedV7_10", "InfectedV8_10", "InfectedV9_10", "InfectedV10_10"},
                    16, 5, fileout);
}
