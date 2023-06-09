#include "tensor.h"

PYBIND11_MODULE(tensorlib, m) {
    py::class_<Tensor>(m, "Tensor", py::buffer_protocol())
        .def_buffer([](Tensor &m) -> py::buffer_info {
            return py::buffer_info(m.data(), sizeof(float),
                                   py::format_descriptor<float>::format(), 2,
                                   {m.rows(), m.cols()},
                                   {sizeof(float) * m.cols(), sizeof(float)});
        })
        .def(py::init<size_t, size_t>())
        .def(py::init<py::array_t<float>>())
        .def("data", &Tensor::data)
        .def("rows", &Tensor::rows)
        .def("cols", &Tensor::cols)
        .def("numpy", &Tensor::numpy, py::return_value_policy::take_ownership)
        .def("__repr__", &Tensor::repr)
        .def("cpu", &Tensor::cpu)
        .def("gpu", &Tensor::gpu)
        .def("onCPU", &Tensor::onCPU)
        .def("gpuFree", &Tensor::gpuFree);

    m.def("cpu_add", [](Tensor *a, Tensor *b) { return cpu_add(a, b); },
          py::return_value_policy::take_ownership);
    m.def("gpu_add", [](Tensor *a, Tensor *b) { return gpu_add(a, b); },
          py::return_value_policy::take_ownership);

    m.def("cpu_sub", [](Tensor *a, Tensor *b) { return cpu_sub(a, b); },
          py::return_value_policy::take_ownership);
    m.def("gpu_sub", [](Tensor *a, Tensor *b) { return gpu_sub(a, b); },
          py::return_value_policy::take_ownership);

    m.def("cpu_neg", [](Tensor *a) { return cpu_neg(a); },
          py::return_value_policy::take_ownership);
    m.def("gpu_neg", [](Tensor *a) { return gpu_neg(a); },
          py::return_value_policy::take_ownership);

    m.def("cpu_mul", [](Tensor *a, Tensor *b) { return cpu_mul(a, b); },
          py::return_value_policy::take_ownership);
    m.def("gpu_mul", [](Tensor *a, Tensor *b) { return gpu_mul(a, b); },
          py::return_value_policy::take_ownership);

    m.def("cpu_div", [](Tensor *a, Tensor *b) { return cpu_div(a, b); },
          py::return_value_policy::take_ownership);
    m.def("gpu_div", [](Tensor *a, Tensor *b) { return gpu_div(a, b); },
          py::return_value_policy::take_ownership);

    m.def("cpu_sum", [](Tensor *a, int axis) { return cpu_sum(a, axis); },
          py::return_value_policy::take_ownership);
    m.def("gpu_sum", [](Tensor *a, int axis) { return gpu_sum(a, axis); },
          py::return_value_policy::take_ownership);

    m.def("cpu_max", [](Tensor *a, int axis) { return cpu_max(a, axis); },
          py::return_value_policy::take_ownership);
    m.def("gpu_max", [](Tensor *a, int axis) { return gpu_max(a, axis); },
          py::return_value_policy::take_ownership);

    m.def("cpu_axial_mask",
          [](Tensor *a, Tensor *idx, int axis) {
              return cpu_axial_mask(a, idx, axis);
          },
          py::return_value_policy::take_ownership);
    m.def("gpu_axial_mask",
          [](Tensor *a, Tensor *idx, int axis) {
              return gpu_axial_mask(a, idx, axis);
          },
          py::return_value_policy::take_ownership);

    m.def("cpu_bct",
          [](Tensor *a, int axis, int dim) { return cpu_bct(a, axis, dim); },
          py::return_value_policy::take_ownership);
    m.def("gpu_bct",
          [](Tensor *a, int axis, int dim) { return gpu_bct(a, axis, dim); },
          py::return_value_policy::take_ownership);

    m.def("cpu_set_zero", [](Tensor *a) { return cpu_set_zero(a); },
          py::return_value_policy::take_ownership);
    m.def("gpu_set_zero", [](Tensor *a) { return gpu_set_zero(a); },
          py::return_value_policy::take_ownership);

    m.def("cpu_cpy", [](Tensor *a) { return cpu_cpy(a); },
          py::return_value_policy::take_ownership);
    m.def("gpu_cpy", [](Tensor *a) { return gpu_cpy(a); },
          py::return_value_policy::take_ownership);

    m.def("cpu_exp", [](Tensor *a) { return cpu_exp(a); },
          py::return_value_policy::take_ownership);
    m.def("gpu_exp", [](Tensor *a) { return gpu_exp(a); },
          py::return_value_policy::take_ownership);

    m.def("cpu_log", [](Tensor *a) { return cpu_log(a); },
          py::return_value_policy::take_ownership);
    m.def("gpu_log", [](Tensor *a) { return gpu_log(a); },
          py::return_value_policy::take_ownership);

    m.def("cpu_tsp", [](Tensor *a) { return cpu_tsp(a); },
          py::return_value_policy::take_ownership);
    m.def("gpu_tsp", [](Tensor *a) { return gpu_tsp(a); },
          py::return_value_policy::take_ownership);

    m.def("cpu_pow", [](Tensor *a, float val) { return cpu_pow(a, val); },
          py::return_value_policy::take_ownership);
    m.def("gpu_pow", [](Tensor *a, float val) { return gpu_pow(a, val); },
          py::return_value_policy::take_ownership);

    m.def("cpu_relu", [](Tensor *a) { return cpu_relu(a); },
          py::return_value_policy::take_ownership);
    m.def("gpu_relu", [](Tensor *a) { return gpu_relu(a); },
          py::return_value_policy::take_ownership);

    m.def("cpu_relu_grad",
          [](Tensor *a, Tensor *grad) { return cpu_relu_grad(a, grad); },
          py::return_value_policy::take_ownership);
    m.def("gpu_relu_grad",
          [](Tensor *a, Tensor *grad) { return gpu_relu_grad(a, grad); },
          py::return_value_policy::take_ownership);

    m.def("cpu_matmul", [](Tensor *a, Tensor *b) { return cpu_matmul(a, b); },
          py::return_value_policy::take_ownership);
    m.def("gpu_matmul", [](Tensor *a, Tensor *b) { return gpu_matmul(a, b); },
          py::return_value_policy::take_ownership);
}