ndarray<2, float> ratings = {"ratings_file"};
ndarray<2, float> L = {ratings.shape[0], K};
ndarray<2, float> R = {ratings.shape[1], K};

float eta = 0.1;
float lambda = 0.01;
int8 K = 100;

def
float eval_rmse() {
  float error = 0;
  int64 num_samples = 0;
  float rmse = 0;
  foreach ( auto e : ratings) {
    float pred = L[e.idx[0]].dot_prod(R[e.idx[1]].transpose());
    error += (pred - e.val) ** 2;
    num_samples++;
  }

  rmse = sqrt(error) / num_samples;
  return rmse;
}

@evaluate "RMSE" @minimize eval_rmse 1;

def
void main() {
  iterate_for (100) {
    foreach (auto e : ratings) {
      float pred = L[e.idx[0]].dot_prod(R[e.idx[1]].transpose());
      float gradc = -2 * (pred - e.val);
      auto L_grad = gradc * R[e.idx[1]] + lambda * L[e.idx[0]];
      auto R_grad = gradc * L[e.idx[0]] + lambda * R[e.idx[1]];
      L[e.idx[0]] -= eta * L_grad;
      R[e.idx[1]] -= eta * R_grad;
    }
  }
}
