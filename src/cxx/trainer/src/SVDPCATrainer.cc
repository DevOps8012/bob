/**
 * @author Laurent El-Shafey <Laurent.El-Shafey@idiap.ch>
 * @author Andre Anjos <andre.anjos@idiap.ch>
 * @date Fri 10 Jun 2011 09:21:16 CEST
 *
 * @brief Principal Component Analysis implemented with Singular Value
 * Decomposition (lapack). Implementation.
 */

#include <vector>
#include <algorithm>

#include "trainer/SVDPCATrainer.h"
#include "math/svd.h"
#include "io/Exception.h"
#include "core/array_type.h"

namespace io = Torch::io;
namespace mach = Torch::machine;
namespace train = Torch::trainer;

train::SVDPCATrainer::SVDPCATrainer(bool zscore_convert)
  : m_zscore_convert(zscore_convert) {
  }

train::SVDPCATrainer::SVDPCATrainer()
  : m_zscore_convert(false) {
  }

train::SVDPCATrainer::SVDPCATrainer(const train::SVDPCATrainer& other)
  : m_zscore_convert(other.m_zscore_convert) {
  }

train::SVDPCATrainer::~SVDPCATrainer() {}

train::SVDPCATrainer& train::SVDPCATrainer::operator=
(const train::SVDPCATrainer& other) {
  m_zscore_convert = other.m_zscore_convert;
  return *this;
}

void train::SVDPCATrainer::train(Torch::machine::LinearMachine& machine, 
    blitz::Array<double,1>& eigen_values, const io::Arrayset& ar) const {

  // checks for arrayset data type and shape once
  if (ar.getElementType() != Torch::core::array::t_float64) {
    throw Torch::io::TypeError(ar.getElementType(),
        Torch::core::array::t_float64);
  }
  if (ar.getNDim() != 1) {
    throw Torch::io::DimensionError(ar.getNDim(), 1);
  }

  // data is checked now and conforms, just proceed w/o any further checks.
  size_t n_samples = ar.size();
  size_t n_features = ar.getShape()[0];

  // loads all the data in a single shot - required for SVD
  blitz::Array<double,2> data(n_features, n_samples);
  blitz::Range all = blitz::Range::all();
  for (size_t i=0; i<n_samples; ++i) {
    data(all,i) = ar.get<double,1>(i);
  }

  // computes the mean of the training data
  blitz::secondIndex j;
  blitz::Array<double,1> mean(n_features);
  mean = blitz::mean(data, j);

  // removes the empirical mean from the training data
  for (size_t i=0; i<n_samples; ++i) data(all,i) -= mean;

  /**
   * computes the singular value decomposition using lapack
   *
   * note: Lapack already organizes the U,Sigma,V**T matrixes so that the
   * singular values in Sigma are organized by decreasing order of magnitude.
   * You **don't** need sorting after this.
   */
  const int n_sigma = std::min(n_features, n_samples);
  blitz::Array<double,2> U(n_features, n_sigma);
  blitz::Array<double,1> sigma(n_sigma);
  Torch::math::svd_(data, U, sigma);

  /**
   * sets the linear machine with the results:
   *
   * note: eigen values are sigma^2/n_samples diagonal
   *       eigen vectors are the rows of U
   */
  machine.resize(n_features, std::min(n_features, n_samples));
  machine.setInputSubtraction(mean);
  machine.setInputDivision(1.0);
  machine.setBiases(0.0);
  U.transposeSelf(1,0);
  machine.setWeights(U);

  //weight normalization (if necessary):
  //norm_factor = blitz::sum(blitz::pow2(V(all,i)))

  // finally, we set also the eigen values in this version
  eigen_values.resize(n_sigma);
  eigen_values = blitz::pow2(sigma)/(n_samples-1);

  // if the user wants z-score normalization
  if (m_zscore_convert) {
    blitz::Array<double,1> tmp(eigen_values.extent(0));
    tmp = blitz::sqrt(eigen_values);
    machine.setInputDivision(tmp);
  }
}

void train::SVDPCATrainer::train(Torch::machine::LinearMachine& machine, 
    const io::Arrayset& ar) const {
  blitz::Array<double,1> throw_away;
  train(machine, throw_away, ar);
}
