import unittest
from epidemiology.secir import (UncertainContactMatrix, ContactFrequencyMatrix, Damping, SecirModel1,
                                simulate, AgeGroup1, InfectionType, SecirSimulation1)

class Test_secir_integration(unittest.TestCase):

    def setUp(self):

        model = SecirModel1()

        model.parameters.times[0].set_incubation(5.2)  # R_2^(-1)+R_3^(-1)
        model.parameters.times[0].set_infectious_mild(6.)  # 4-14  (=R4^(-1))
        model.parameters.times[0].set_serialinterval(4.2)   # 4-4.4 // R_2^(-1)+0.5*R_3^(-1)
        model.parameters.times[0].set_hospitalized_to_home(12.)  # 7-16 (=R5^(-1))
        model.parameters.times[0].set_home_to_hospitalized(5.)  # 2.5-7 (=R6^(-1))
        model.parameters.times[0].set_hospitalized_to_icu(2.)  # 1-3.5 (=R7^(-1))
        model.parameters.times[0].set_icu_to_home(8.)  # 5-16 (=R8^(-1))
        model.parameters.times[0].set_icu_to_death(5.)  # 3.5-7 (=R5^(-1))

        model.parameters.probabilities[0].set_infection_from_contact(1.0)
        model.parameters.probabilities[0].set_asymp_per_infectious(0.09)  # 0.01-0.16
        model.parameters.probabilities[0].set_risk_from_symptomatic(0.25)  # 0.05-0.5
        model.parameters.probabilities[0].set_hospitalized_per_infectious(0.2)  # 0.1-0.35
        model.parameters.probabilities[0].set_icu_per_hospitalized(0.25)  # 0.15-0.4
        model.parameters.probabilities[0].set_dead_per_icu(0.3)  # 0.15-0.77

        model.populations.set(7600, AgeGroup1.Group0, InfectionType.S)
        model.populations.set(100, AgeGroup1.Group0, InfectionType.E)
        model.populations.set(50, AgeGroup1.Group0, InfectionType.C)
        model.populations.set(50, AgeGroup1.Group0, InfectionType.I)
        model.populations.set(20, AgeGroup1.Group0, InfectionType.H)
        model.populations.set(10, AgeGroup1.Group0, InfectionType.U)
        model.populations.set(10, AgeGroup1.Group0, InfectionType.R)
        model.populations.set(0, AgeGroup1.Group0, InfectionType.D)


        model.parameters.get_contact_patterns().get_cont_freq_mat().set_cont_freq(0.5, 0, 0)

        model.apply_constraints()

        self.model = model

    def test_simulate_simple(self):
      result = simulate(t0=0., tmax=100., dt=0.1, model=self.model)
      self.assertAlmostEqual(result.get_time(0), 0.)
      self.assertAlmostEqual(result.get_time(1), 0.1)
      self.assertAlmostEqual(result.get_last_time(), 100.)

    def test_simulation_simple(self):
        sim = SecirSimulation1(self.model, t0 = 0., dt = 0.1)
        sim.advance(tmax = 100.)
        result = sim.result
        self.assertAlmostEqual(result.get_time(0), 0.)
        self.assertAlmostEqual(result.get_time(1), 0.1)
        self.assertAlmostEqual(result.get_last_time(), 100.)

if __name__ == '__main__':
    unittest.main()
