struct SegmentTree {
  using Data = int32_t;
  using QueryRes = int32_t;

#define _MID __mid
#define _LEFT __l
#define _RIGHT __r
#define _ROOT __root
#define _ARGS int _LEFT, int _RIGHT, int _ROOT
#define _GEN_MID int _MID = (_LEFT + _RIGHT) / 2;
#define _LR (_ROOT << 1)
#define _RR (_LR | 1)
#define _LSON _LEFT, _MID, _LR
#define _RSON (_MID + 1), _RIGHT, _RR
#define _CUR _LEFT, _RIGHT, _ROOT

  SegmentTree(size_t n) : da_(n, 0) {}
  static SegmentTree NewFromMaxn(size_t maxn) {

    return SegmentTree(std::pow(2, std::ceil(std::log2(maxn)) + 1));
  }

  void merge_up(_ARGS) {
    if (_LEFT == _RIGHT)
      return;
    // TODO
    // da_[_ROOT] = std::max(da_[_LR], da_[_RR]);
  }

  void update(int from, int to, _ARGS, Data val) {
    if (from <= _LEFT && _RIGHT <= to) {
      // TODO
      // da_[_ROOT] = val;
      return;
    }
    _GEN_MID
    if (_MID >= from)
      update(from, to, _LSON, val);
    if (_MID < to)
      update(from, to, _RSON, val);

    merge_up(_CUR);
  }

  QueryRes query(int from, int to, _ARGS) {
    if (from <= _LEFT && _RIGHT <= to) {
      // TODO
      // return da_[_ROOT];
      return;
    }

    _GEN_MID

    QueryRes ans{};
    if (_MID >= from)
      ans = std::max(query(from, to, _LSON), ans);
    if (_MID < to)
      ans = std::max(query(from, to, _RSON), ans);
    return ans;
  }

#undef _MID
#undef _LEFT
#undef _RIGHT
#undef _ROOT
#undef _ARGS
#undef _GEN_MID
#undef _LR
#undef _RR
#undef _LSON
#undef _RSON
#undef _CUR

  /*
        root from 1
    */
  std::vector<Data> da_;
};
