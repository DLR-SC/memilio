/* 
* Copyright (C) 2020-2023 German Aerospace Center (DLR-SC)
*
* Authors: Rene Schmieding, Henrik Zunker
*
* Contact: Martin J. Kuehn <Martin.Kuehn@DLR.de>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef MIO_FLOW_MODEL_H_
#define MIO_FLOW_MODEL_H_

#include "memilio/compartments/compartmentalmodel.h"

namespace mio
{

namespace details
{
// The following functions are not defined anywhere. Their use is to provide type conversions via their return type.

// Function declaration used to remove OmittedTag from the type list of a tuple.
// First a list of tuples is generated for each Tag in Tags, where the tuple is either of type tuple<Tag>, or if
// Tag == OmittedTag, of type tuple<>. This list is then concatenated, effectively removing OmittedTag.
template <class OmittedTag, class... Tags>
decltype(std::tuple_cat(std::declval<typename std::conditional<std::is_same<OmittedTag, Tags>::value, std::tuple<>,
                                                               std::tuple<Tags>>::type>()...))
    filter_tuple(std::tuple<Tags...>);

// Function declaration used to replace type T by std::tuple.
template <template <class...> class T, class... Args>
std::tuple<Args...> as_tuple(T<Args...>);

// Function declaration used to replace std::tuple by type T.
template <template <class...> class T, class... Args>
T<Args...> as_index(std::tuple<Args...>);

// Remove all occurrences of OmittedTag from the types in a std::tuple<types...>.
template <class OmittedTag, class Tuple>
using filtered_tuple_t = decltype(filter_tuple<OmittedTag>(std::declval<Tuple>()));

// Remove all occurrences of OmittedTag from the types in an Index = IndexTemplate<types...>.
template <class OmittedTag, template <class...> class IndexTemplate, class Index>
using filtered_index_t = decltype(as_index<IndexTemplate>(
    std::declval<filtered_tuple_t<OmittedTag, decltype(as_tuple(std::declval<Index>()))>>()));

} //namespace details

/**
 * @brief A FlowModel is a CompartmentalModel defined by the flows between compartments. 
 *
 * Inherits from  @see CompartmentalModel, and defines the derivatives depending on the flows. Hence, a model implementing
 * FlowModel has to define the function get_flows, instead of get_derivatives.
 *
 * Flows is expected to be a TypeList containing types Flow<A,B>, where A and B are compartments from the enum Comp.
 * Some examples can be found in the cpp/models/ directory, within the model.h files.
 */
template <class Comp, class Pop, class Params, class Flows>
struct FlowModel : public CompartmentalModel<Comp, Pop, Params> {
    using PopIndex = typename Pop::Index;
    // FlowIndex is the same as PopIndex without the category Comp. It is used as argument type for get_flow_index,
    // since a flow is used to determine only the compartment (i.e. Index<Comp>) for the population of the model.
    // The remaining indices (those contained in FlowIndex) must still be provided to get an entry of population.
    // This approach only works with exactly one category of type Comp in PopIndex (hence the assertion below).
    using FlowIndex = details::filtered_index_t<Comp, Index, PopIndex>;
    // Enforce that Comp is a unique Category of PopIndex, since we use Flows (via their source/target) to provide
    // Comp indices for the population.
    static_assert(FlowIndex::size == PopIndex::size - 1, "Compartments must be used exactly once as population index.");

public:
    using Base = CompartmentalModel<Comp, Pop, Params>;
    /**
     * @brief Default constructor, forwarding args to Base constructor.
     */
    template <class... Args>
    FlowModel(Args... args)
        : CompartmentalModel<Comp, Pop, Params>(args...)
        , m_flow_index_dimensions(reduce_index<FlowIndex>(this->populations.size()))
        , m_flow_range([&]() {
            // take population dimensions, and set the compartment dimension to 1. this allows us to effectively
            // iterate over all flow indices, but still have the corresponding PopIndex, where we can insert the
            // flow source or target.
            PopIndex dims   = this->populations.size();
            get<Comp>(dims) = Index<Comp>{1};
            return MultiIndexRange<PopIndex>(dims);
        }())
        , m_flow_values((this->populations.numel() / static_cast<size_t>(Comp::Count)) * Flows::size())
    {
    }

    // Note: use get_flow_index when accessing flows
    // Note: by convention, we compute incoming flows, thus entries in flows must be non-negative
    virtual void get_flows(Eigen::Ref<const Eigen::VectorXd>, Eigen::Ref<const Eigen::VectorXd> /*y*/, double /*t*/,
                           Eigen::Ref<Eigen::VectorXd> /*flows*/) const {};

    /**
     * @brief Compute the right-hand-side of the ODE dydt = f(y, t) from flow values.
     *
     * This function is generated at compile time depending on the template parameters Flows and Pop of the model.
     *
     * @param[in] flows The current flow values (as calculated by get_flows) as a flat array.
     * @param[out] dydt A reference to the calculated output.
     */
    void get_derivatives(Eigen::Ref<const Eigen::VectorXd> flows, Eigen::Ref<Eigen::VectorXd> dydt) const
    {
        // set dydt to 0, then iteratively add all flow contributions
        dydt.setZero();
        for (PopIndex I : m_flow_range) { // take I by value, so we can safely manipulate it
            get_rhs_impl(flows, dydt, I);
        }
    }

    /**
     * @brief Compute the right-hand-side f(y, t) of the ODE and store it in dydt.
     *
     * This function uses get_flow(..., flows) and get_derivatives(flows, dydt) to provide the
     * same interface as a CompartmentalModel.
     *
     * @param[in] pop The current population of the model as a flat array.
     * @param[in] y The current state of the model as a flat array.
     * @param[in] t The current time.
     * @param[out] dydt A reference to the calculated output.
     */
    void get_derivatives(Eigen::Ref<const Eigen::VectorXd> pop, Eigen::Ref<const Eigen::VectorXd> y, double t,
                         Eigen::Ref<Eigen::VectorXd> dydt) const override final
    {
        m_flow_values.setZero();
        get_flows(pop, y, t, m_flow_values);
        get_derivatives(m_flow_values, dydt);
    }

    /**
     * @brief Initial values for flows.
     * This can be used as initial conditions in an ODE solver. By default, this is a zero vector.
     * @return The initial flows.
     */
    Eigen::VectorXd get_initial_flows() const
    {
        return Eigen::VectorXd::Zero((this->populations.numel() / static_cast<size_t>(Comp::Count)) * Flows::size());
    }

    /**
     * @brief A flat index into an array of flows (as computed by get_flows), given the indices of each category.
     *
     * Calculation of flat indices (as in this->populations.get_flat_index) use the formula
     * flat_index = ((((I_{1}) * D_{2} + I_{2}) * D_{3} + I_{3}) ... ) * D_{n} + I_{n}
     * = I_{n} + I_{n-1} * D_{n} + I_{n-2} * D_{n} * D_{n-1} + ... + I_{1} * D_{n} * ... * D_{2}
     * for indices (I_{1}, ... , I_{n}) and dimension (D_{1}, ... , D_{n}).
     *   
     * The flows use their position in the FlowChart instead of the template argument Comp from the base class,
     * i.e. they use the Population::Index type without Comp. The position is determined by the Source and Target 
     * template arguments instead. As this dimension (the number of flows) may be larger or smaller
     * (e.g. a S->I->R model has 3 Comp's, but 2 Flows) than Comp::Count, we cannot simply use
     * this->populations.get_flat_index.
     *   
     * Instead, we omit Comp from PopIndex (getting FlowIndex), calculate the flat_index without the Comp index or
     * Dimension, i.e. we omit I_j and D_j corresponding to Comp from the formula above. We do this by calling
     * flatten_index with a FlowIndex, with dimensions derived from Pop via reduce_index.
     * Then, we add the flow position I_{flow} via flow_index = flat_index * D_{flow} + I_{flow}, where
     * D_{flow} is the number of flows.
     *
     * @param[in] indices The custom indices for each category.
     * @tparam Source the source of a flow.
     * @tparam Target the target of a flow.
     * @return A flat index into a data structure storing flow values.
     */
    template <Comp Source, Comp Target>
    size_t get_flat_flow_index(const FlowIndex& indices) const
    {
        if constexpr (std::is_same_v<FlowIndex, Index<>>) {
            return get_flat_flow_index<Source, Target>();
        }
        else {
            return flatten_index(indices, m_flow_index_dimensions) * Flows::size() +
                   index_of_type_v<Flow<Source, Target>, Flows>;
        }
    }

    /**
     * @brief A flat index into an array of flows (as computed by get_flows), if the only used category in Pop is Comp.
     * @tparam Source the source of a flow.
     * @tparam Target the target of a flow.
     * @return A flat index into a data structure storing flow values.
     */
    template <Comp Source, Comp Target>
    constexpr size_t get_flat_flow_index() const
    {
        static_assert(std::is_same<FlowIndex, Index<>>::value, "Other indices must be specified");
        return index_of_type_v<Flow<Source, Target>, Flows>;
    }

private:
    FlowIndex m_flow_index_dimensions; ///< The dimensions of a FlowIndex.
    const MultiIndexRange<PopIndex> m_flow_range; ///< Range used to iterate over all categories except for Comp.
    mutable Eigen::VectorXd m_flow_values; ///< Cache to avoid allocation in get_derivatives (using get_flows).

    /**
     * @brief Compute the derivatives of the compartments.
     * Compute the derivatives/rhs of the model equations for a given flow and index.
     * Uses recursion, which is resolved at compile time depending on the Flows.
     * @param[in] flows Current change in flows, as computed by get_flows.
     * @param[out] rhs The derivatives of the model equations.
     * @param[in, out] i The Popindex at which to compute derivative. The entry for Comp wll be overwritten.
     * @tparam I The index of a flow in FlowChart.
     */
    template <size_t I = 0>
    inline void get_rhs_impl(Eigen::Ref<const Eigen::VectorXd> flows, Eigen::Ref<Eigen::VectorXd> rhs,
                             PopIndex& index) const
    {
        using Flow = type_at_index_t<I, Flows>;
        // subtract outflow from source compartment
        get<Comp>(index) = Flow::source;
        rhs[Base::populations.get_flat_index(index)] -=
            flows[get_flat_flow_index<Flow::source, Flow::target>(reduce_index<FlowIndex>(index))];
        // add outflow to target compartment
        get<Comp>(index) = Flow::target;
        rhs[Base::populations.get_flat_index(index)] +=
            flows[get_flat_flow_index<Flow::source, Flow::target>(reduce_index<FlowIndex>(index))];
        // handle next flow (if there is one)
        if constexpr (I + 1 < Flows::size()) {
            get_rhs_impl<I + 1>(flows, rhs, index);
        }
    }
};

} // namespace mio

#endif // MIO_FLOW_MODEL_H_