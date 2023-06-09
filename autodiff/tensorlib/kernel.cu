#include "tensor.h"

__global__ void _add(float *a, float *b, float *res, int dim0, int dim1) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < dim0 * dim1) {
        res[idx] = a[idx] + b[idx];
    }
}

__global__ void _sub(float *a, float *b, float *res, int dim0, int dim1) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < dim0 * dim1) {
        res[idx] = a[idx] - b[idx];
    }
}

__global__ void _neg(float *a, float *res, int dim0, int dim1) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < dim0 * dim1) {
        res[idx] = -a[idx];
    }
}

__global__ void _mul(float *a, float *b, float *res, int dim0, int dim1) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < dim0 * dim1) {
        res[idx] = a[idx] * b[idx];
    }
}

__global__ void _div(float *a, float *b, float *res, int dim0, int dim1) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < dim0 * dim1) {
        res[idx] = a[idx] / b[idx];
    }
}

__global__ void _relu(float *a, float *res, int dim0, int dim1) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < dim0 * dim1) {
        res[idx] = a[idx] > 0 ? a[idx] : 0;
    }
}
__global__ void _relu_grad(float *a, float *grad, float *res, int dim0,
                           int dim1) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < dim0 * dim1) {
        res[idx] = a[idx] > 0 ? grad[idx] : 0;
    }
}

__global__ void _sum(float *a, float *res, int dim0, int dim1, int axis) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    int res_dim0, res_dim1;
    if (axis == 0) {
        res_dim0 = 1;
        res_dim1 = dim1;
    } else {
        res_dim0 = dim0;
        res_dim1 = 1;
    }

    if (idx < res_dim0 * res_dim1) {
        float temp_result = 0;
        if (axis == 0) {
            for (int i = 0; i < dim0; i++) {
                temp_result += a[i * dim1 + idx];
            }
        } else {
            for (int j = 0; j < dim1; j++) {
                temp_result += a[idx * dim1 + j];
            }
        }
        res[idx] = temp_result;
    }
}

__global__ void _max(float *a, float *res, float *id, int dim0, int dim1,
                     int axis) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    int res_dim0, res_dim1;
    if (axis == 0) {
        res_dim0 = 1;
        res_dim1 = dim1;
    } else {
        res_dim0 = dim0;
        res_dim1 = 1;
    }

    if (idx < res_dim0 * res_dim1) {
        float temp_result;
        float index = 0;
        if (axis == 0) {
            temp_result = a[idx];
            for (int i = 0; i < dim0; i++) {
                if (temp_result < a[i * dim1 + idx]) {
                    temp_result = a[i * dim1 + idx];
                    index = i;
                }
            }
        } else {
            temp_result = a[idx * dim1];
            for (int j = 0; j < dim1; j++) {
                if (temp_result < a[idx * dim1 + j]) {
                    temp_result = a[idx * dim1 + j];
                    index = j;
                }
            }
        }
        res[idx] = temp_result;
        id[idx] = index;
    }
}

__global__ void _axial_mask(float *res, float *id, int dim0, int dim1,
                           int axis) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    int res_dim0, res_dim1;
    if (axis == 0) {
        res_dim0 = 1;
        res_dim1 = dim1;
    } else {
        res_dim0 = dim0;
        res_dim1 = 1;
    }

    if (idx < res_dim0 * res_dim1) {
        if (axis == 0) {
            res[(int)id[idx] * dim1 + idx] = 1.0;
        } else {
            res[(int)id[idx] + dim1 * idx] = 1.0;
        }
    }
}

__global__ void _bct(float *a, float *res, int res_dim0, int res_dim1,
                     int axis) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < res_dim0 * res_dim1) {
        if (axis == 0)
            res[idx] = a[idx % res_dim1];
        else {
            res[idx] = a[idx / res_dim1];
        }
    }
}

__global__ void _exp(float *a, float *res, int dim0, int dim1) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < dim0 * dim1) {
        res[idx] = exp(a[idx]);
    }
}

__global__ void _log(float *a, float *res, int dim0, int dim1) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < dim0 * dim1) {
        res[idx] = log(a[idx]);
    }
}

__global__ void _tsp(float *a, float *res, int dim0, int dim1) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    unsigned int i = idx % dim1;
    unsigned int j = idx / dim1;

    if (idx < dim0 * dim1) {
        res[j + i * dim0] = a[idx];
    }
}

__global__ void _pow(float *a, float *res, int dim0, int dim1, float val) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < dim0 * dim1) {
        res[idx] = pow(a[idx], val);
    }
}

__global__ void _matmul(float *a, float *b, float *res, int dim0_a, int dim1_a,
                        int dim1_b) {
    unsigned int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < dim0_a * dim1_b) {
        int row = idx / dim1_b;
        int col = idx % dim1_b;
        float sum = 0.0;

        for (int i = 0; i < dim1_a; i++) {
            sum += a[row * dim1_a + i] * b[i * dim1_b + col];
        }

        res[idx] = sum;
    }
}