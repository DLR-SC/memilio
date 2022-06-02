#include "utils/time_series.h"
#include "memilio/utils/time_series.h"

#include "pybind11/eigen.h"

namespace pymio
{

void bind_time_series(pybind11::module& m, std::string const& name)
{
    pybind11::class_<mio::TimeSeries<double>>(m, "TimeSeries")
        .def(pybind11::init<Eigen::Index>(), pybind11::arg("num_elements"))
        .def("get_num_time_points", &mio::TimeSeries<double>::get_num_time_points)
        .def("get_num_elements", &mio::TimeSeries<double>::get_num_elements)
        .def("get_time", pybind11::overload_cast<Eigen::Index>(&mio::TimeSeries<double>::get_time), pybind11::arg("index"))
        .def("get_last_time", pybind11::overload_cast<>(&mio::TimeSeries<double>::get_last_time))
        .def("get_value", pybind11::overload_cast<Eigen::Index>(&mio::TimeSeries<double>::get_value), pybind11::arg("index"))
        .def("get_last_value", pybind11::overload_cast<>(&mio::TimeSeries<double>::get_last_value))
        .def("__len__", &mio::TimeSeries<double>::get_num_time_points)
        .def(
            "__getitem__",
            [](mio::TimeSeries<double>& self, Eigen::Index i) {
                if (i >= 0 && i < self.get_num_time_points()) {
                    return self[i];
                }
                else {
                    throw pybind11::index_error("Index out of range."); //needs to throw exception for iterable
                }
            },
            pybind11::is_operator(), pybind11::arg("index"))
        .def(
            "__setitem__",
            [](mio::TimeSeries<double>& self, Eigen::Index i, Eigen::Ref<const mio::TimeSeries<double>::Vector> expr) {
                if (i >= 0 && i < self.get_num_time_points()) {
                    self[i] = expr;
                }
                else {
                    throw pybind11::index_error("Index out of range."); //needs to throw exception for iterable
                }
            },
            pybind11::is_operator(), pybind11::arg("index"), pybind11::arg("v"))
        .def("add_time_point",
             [](mio::TimeSeries<double>& self) {
                 return self.add_time_point();
             })
        .def("add_time_point",
             [](mio::TimeSeries<double>& self, double t) {
                 return self.add_time_point(t);
             })
        .def("add_time_point",
             [](mio::TimeSeries<double>& self, double t, Eigen::Ref<const mio::TimeSeries<double>::Vector> expr) {
                 return self.add_time_point(t, expr);
             })
        .def("as_ndarray", [](mio::TimeSeries<double>& self) {
            auto m = Eigen::Map<mio::TimeSeries<double>::Matrix>(self.data(), self.get_num_rows(),
                                                                 self.get_num_time_points());
            return Eigen::Ref<mio::TimeSeries<double>::Matrix>(m);
        });
}

} // namespace pymio