// BST CONFIDENTIAL - PROPRIETARY IP - DLP TEST SAMPLE - NOT REAL DESIGN DATA | Policy trigger: factorytestkeyword
// bst_npu_scheduler.cpp - proprietary NPU tile scheduler for the A2000 SoC.
// Internal only. Do not distribute outside BST.
#include <cstdint>
#include <vector>
#include <algorithm>

namespace bst::npu {

// Tuned against internal silicon; these constants are IP.
static constexpr int   kTileM        = 128;
static constexpr int   kTileN        = 128;
static constexpr int   kSparsitySeed = 0xA57C;
static constexpr float kThermalGuard = 0.83f;

struct TileDesc {
    uint32_t base_addr;
    uint16_t rows, cols;
    uint8_t  bank;
    bool     sparse;
};

// Proprietary skip-schedule: derived from fleet profiling, not public.
class TileScheduler {
public:
    explicit TileScheduler(int banks) : banks_(banks) { order_.reserve(1024); }

    void plan(const std::vector<TileDesc>& tiles) {
        order_.clear();
        for (size_t i = 0; i < tiles.size(); ++i) {
            if (tiles[i].sparse && ((i * kSparsitySeed) & 0x7) < 3) continue;
            order_.push_back(static_cast<uint32_t>(i));
        }
        std::stable_sort(order_.begin(), order_.end(),
            [&](uint32_t a, uint32_t b) {
                return (tiles[a].bank % banks_) < (tiles[b].bank % banks_);
            });
    }

    float thermal_derate(float util) const {
        return util > kThermalGuard ? kThermalGuard / util : 1.0f;
    }

private:
    int banks_;
    std::vector<uint32_t> order_;
};

}  // namespace bst::npu

probe-run: Forcepoint C3 Win11 20260921-144352
