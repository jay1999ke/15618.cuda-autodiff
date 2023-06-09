#include <cmath>
#include <cuda_runtime.h>
#include <iomanip>
#include <iostream>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>

namespace py = pybind11;

class Tensor {
  public:
    Tensor(size_t rows, size_t cols, bool set_zero = false)
        : dim0(rows), dim1(cols) {
        if (set_zero)
            cpu_data = new float[rows * cols]();
        else
            cpu_data = new float[rows * cols];
        on_gpu = false;
        gpu_data = nullptr;
    }

    // creating an object from numpy array (expected to be on cpu)
    Tensor(py::array_t<float> numpy) {
        py::buffer_info info = numpy.request();
        auto orig_data = info.ptr;
        assert(info.shape.size() == 2);
        dim0 = info.shape[0];
        dim1 = info.shape[1];
        on_gpu = false;
        gpu_data = nullptr;

        cpu_data = new float[rows() * cols()];
        memcpy(cpu_data, orig_data, size());
    }

    ~Tensor() {
        delete[] cpu_data;
        if (on_gpu)
            gpuFree();
    };

    // buffer ops
    float *data() { return cpu_data; }
    size_t rows() const { return dim0; }
    size_t cols() const { return dim1; }

    // python api
    void gpu();
    void cpu();
    void gpuFree();
    bool onCPU() { return !on_gpu; };
    std::string repr();
    py::buffer_info request() {
        return py::buffer_info(
            data(), sizeof(float), py::format_descriptor<float>::format(), 2,
            {rows(), cols()}, {sizeof(float) * cols(), sizeof(float)});
    };
    py::array_t<float> numpy() {
        onCpuAssert();
        auto info = py::buffer_info( // make a copy
            nullptr, sizeof(float), py::format_descriptor<float>::format(), 2,
            {rows(), cols()}, {sizeof(float) * cols(), sizeof(float)});
        auto result = py::array(info);
        auto res_ptr = static_cast<float *>(result.request().ptr);
        memcpy(res_ptr, cpu_data, size());
        return result;
    }

    // cpp internal ops
    float *dataGpu() { return gpu_data; }
    void gpu_alloc();
    void maintain();
    void setOnGpu(bool);
    size_t size() const { return dim0 * dim1 * sizeof(float); };

    // safetly assertions
    void onCpuAssert() {
        if (on_gpu == true)
            throw std::runtime_error("Tensor on GPU; Should be on CPU");
    };
    void onGpuAssert() {
        if (on_gpu == false)
            throw std::runtime_error("Tensor on CPU; Should be on GPU");
    };
    void sameShapeAssert(Tensor *other) {
        if (rows() != other->rows() || cols() != other->cols())
            throw std::runtime_error("Tensor shapes don't match");
    }

  private:
    size_t dim0, dim1;
    float *cpu_data;
    bool on_gpu;
    float *gpu_data;
};

// gpu helpers
Tensor *createGPUTensor(size_t rows, size_t cols);

// cpu arith ops
void cpu_set_zero(Tensor *a);
Tensor *cpu_add(Tensor *a, Tensor *b);
Tensor *cpu_sub(Tensor *a, Tensor *b);
Tensor *cpu_neg(Tensor *a);
Tensor *cpu_mul(Tensor *a, Tensor *b);
Tensor *cpu_div(Tensor *a, Tensor *b);
Tensor *cpu_sum(Tensor *a, int axis);
Tensor *cpu_bct(Tensor *a, int axis, int dim);
Tensor *cpu_cpy(Tensor *a);
Tensor *cpu_exp(Tensor *a);
Tensor *cpu_log(Tensor *a);
Tensor *cpu_tsp(Tensor *a);
Tensor *cpu_pow(Tensor *a, float val);
Tensor *cpu_relu(Tensor *a);
Tensor *cpu_relu_grad(Tensor *a, Tensor *grad);
Tensor *cpu_matmul(Tensor *a, Tensor *b);
std::vector<Tensor *> cpu_max(Tensor *a, int axis);
Tensor *cpu_axial_mask(Tensor *a, Tensor *idx, int axis);

// gpu arith ops
void gpu_set_zero(Tensor *a);
Tensor *gpu_add(Tensor *a, Tensor *b);
Tensor *gpu_sub(Tensor *a, Tensor *b);
Tensor *gpu_neg(Tensor *a);
Tensor *gpu_mul(Tensor *a, Tensor *b);
Tensor *gpu_div(Tensor *a, Tensor *b);
Tensor *gpu_sum(Tensor *a, int axis);
Tensor *gpu_bct(Tensor *a, int axis, int dim);
Tensor *gpu_cpy(Tensor *a);
Tensor *gpu_exp(Tensor *a);
Tensor *gpu_log(Tensor *a);
Tensor *gpu_tsp(Tensor *a);
Tensor *gpu_pow(Tensor *a, float val);
Tensor *gpu_relu(Tensor *a);
Tensor *gpu_relu_grad(Tensor *a, Tensor *grad);
Tensor *gpu_matmul(Tensor *a, Tensor *b);
std::vector<Tensor *> gpu_max(Tensor *a, int axis);
Tensor *gpu_axial_mask(Tensor *a, Tensor *idx, int axis);

// kernels
__global__ void _add(float *a, float *b, float *res, int dim0, int dim1);
__global__ void _sub(float *a, float *b, float *res, int dim0, int dim1);
__global__ void _neg(float *a, float *res, int dim0, int dim1);
__global__ void _mul(float *a, float *b, float *res, int dim0, int dim1);
__global__ void _div(float *a, float *b, float *res, int dim0, int dim1);
__global__ void _sum(float *a, float *res, int dim0, int dim1, int axis);
__global__ void _bct(float *a, float *res, int res_dim0, int res_dim1,
                     int axis);
__global__ void _exp(float *a, float *res, int dim0, int dim1);
__global__ void _log(float *a, float *res, int dim0, int dim1);
__global__ void _tsp(float *a, float *res, int dim0, int dim1);
__global__ void _pow(float *a, float *res, int dim0, int dim1, float val);
__global__ void _relu(float *a, float *res, int dim0, int dim1);
__global__ void _relu_grad(float *a, float *grad, float *res, int dim0,
                           int dim1);
__global__ void _matmul(float *a, float *b, float *res, int dim0_a, int dim1_a,
                        int dim1_b);
__global__ void _axial_mask(float *res, float *id, int dim0, int dim1,
                           int axis);
__global__ void _max(float *a, float *res, float *id, int dim0, int dim1,
                     int axis);