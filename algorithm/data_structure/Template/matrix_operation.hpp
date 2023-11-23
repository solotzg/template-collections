template <typename T, int R, int C> struct Matrix {
  using Row = std::array<T, C>;
  using Data = std::array<Row, R>;

  int r_, c_;
  Data data_;

  int rows() const { return r_; }
  int cols() const { return c_; }

  Matrix(const Data &da, int r, int c) : r_(r), c_(c), data_(da) {}
  Matrix(const int r, const int c, bool is_one = false) : r_(r), c_(c) {
    std::memset(&data_, 0, sizeof(data_));
    if (is_one)
      for (int i = 0; i < r_; ++i)
        data_[i][i] = T(1);
  }
  template <typename TUP> Matrix multi(const Matrix &a, T mod) const {
    Matrix res(r_, a.c_);
    for (int i = 0; i < res.r_; ++i)
      for (int j = 0; j < res.c_; ++j)
        for (int k = 0; k < c_; ++k) {
          res.data_[i][j] =
              ((TUP)(data_[i][k]) * a.data_[k][j] + res.data_[i][j]) % mod;
        }
    return res;
  }
  const T *operator[](const int &i) const { return data_[i]; }
  T *operator[](int i) { return data_[i]; }
  template <typename TUP> Matrix pow(int64_t n, T mod) {
    Matrix res(r_, c_, 1), p = *this;
    while (n) {
      if (n & 1)
        res = res.multi<TUP>(p, mod);
      p = p.multi<TUP>(p, mod);
      n >>= 1;
    }
    return res;
  }
  template <typename TUP> static T Fibonacci(uint64_t n, T mod) {
    constexpr int N = 2;
    static_assert(R == N);
    static_assert(C == N);

    constexpr Data FIBO = {Row{0, 1}, Row{0, 0}};
    constexpr Data MUL = {Row{0, 1}, Row{1, 1}};

    Matrix bg(FIBO, N, N);
    bg = bg.multi<TUP>(Matrix(MUL, N, N).pow<TUP>(n, mod), mod);
    return bg.data_[0][0];
  }
};
