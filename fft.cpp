#include <fft.h>

#include <cmath>
#include <fftw3.h>

constexpr size_t kWidth = 8;
constexpr size_t kNormalize = 16;

class DctCalculator::Impl {
public:
    Impl(size_t width, std::vector<double>* input, std::vector<double>* output)
        : input_(input), output_(output), width_(width) {
    }

    std::vector<double>* GetInput() const {
        return input_;
    }

    std::vector<double>* GetOutput() const {
        return output_;
    }

    size_t GetWidth() const {
        return width_;
    }

private:
    std::vector<double>* input_;
    std::vector<double>* output_;
    size_t width_;
};

DctCalculator::DctCalculator(size_t width, std::vector<double>* input,
                             std::vector<double>* output) {
    if (input->size() != width * width || input->size() != output->size()) {
        throw std::invalid_argument("Invalid sizes");
    } else if (width != kWidth) {
        throw std::invalid_argument("Invalid width");
    }
    impl_ = std::make_unique<Impl>(width, input, output);
}

void DctCalculator::Inverse() {
    size_t i = 0;
    while (i != impl_->GetWidth()) {
        impl_->GetInput()->data()[i] *= sqrt(2);
        ++i;
    }
    i = 0;
    while (i != impl_->GetWidth() * impl_->GetWidth()) {
        impl_->GetInput()->data()[i] *= sqrt(2);
        i += kWidth;
    }
    fftw_plan plan =
        fftw_plan_r2r_2d(impl_->GetWidth(), impl_->GetWidth(), impl_->GetInput()->data(),
                         impl_->GetOutput()->data(), FFTW_REDFT01, FFTW_REDFT01, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    fftw_cleanup();
    for (auto& x : *(impl_->GetOutput())) {
        x /= kNormalize;
    }
}

DctCalculator::~DctCalculator() = default;
