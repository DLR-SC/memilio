#include "epidemiology/abm/abm.h"

int main()
{
    auto world    = epi::World();
    auto& home    = world.add_node(epi::NodeType::Home);
    auto& school  = world.add_node(epi::NodeType::School);
    auto& work    = world.add_node(epi::NodeType::Work);
    auto& child1  = world.add_person(home, epi::InfectionState::Susceptible);
    auto& child2  = world.add_person(home, epi::InfectionState::Susceptible);
    auto& parent1 = world.add_person(home, epi::InfectionState::Carrier);
    auto& parent2 = world.add_person(home, epi::InfectionState::Susceptible);

    auto t0   = 0;
    auto tmax = 100;
    auto sim  = epi::AbmSimulation(t0, std::move(world));

    sim.advance(tmax);

    std::cout << "Ran ABM from " << t0 << " to " << 100 << '\n';
    for (auto&& v : sim.get_result()) {
        std::cout << v.transpose() << '\n';
    }
    std::cout << std::endl;
}