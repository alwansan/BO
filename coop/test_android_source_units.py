"""Metadata compatibility calibration; no game/SDL/Android toolchain needed."""
import unittest
from android_source_units import symbols


class SymbolMetadataTests(unittest.TestCase):
    def test_matches_pinned_codegen_optional_functions_data_and_abi(self):
        # Missing functions must NOT all alias zero or each other. Missing
        # data is zero, not a fabricated callable placeholder. Order is the
        # original C++ std::map order, not TOML insertion order.
        config = {"symbols": {"z": 0, "middle": 0x26AAE0, "a": 0},
                  "data": {"null_data": 0}, "layout": {"Slot": 4},
                  "abi": {"middle": ["eax", "stack"]}}
        text = symbols(config)
        self.assertIn("#define SYM_a FFFF0000\n#define HAS_a 0", text)
        self.assertIn("#define SYM_z FFFF0001\n#define HAS_z 0", text)
        self.assertIn("#define SYM_middle 0026AAE0\n#define HAS_middle 1", text)
        self.assertIn("#define SYM_null_data 00000000\n#define HAS_null_data 0", text)
        self.assertIn("#define XDK_Slot 0x4", text)
        self.assertIn("#define IMAGE_ABI(X) X(middle, eax, stack)", text)
