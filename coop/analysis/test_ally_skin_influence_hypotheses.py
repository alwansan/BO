#!/usr/bin/env python3
"""Strict synthetic-only test fixture; no original owner MODEL bytes."""
import unittest
from ally_skin_influence_hypotheses import HYPOTHESES,pair_counts,stage_scan

class Hypotheses(unittest.TestCase):
    def test_conditional_active_influences_and_negative_control(self):
        data=bytearray(36*3)
        for i,(w,b) in enumerate([
            ((255,0,0,0),(3,255,255,255)),
            ((128,127,0,0),(3,4,255,255)),
            ((100,100,55,0),(3,35,4,255))]):
            data[i*36+12:i*36+16]=bytes(w)
            data[i*36+20:i*36+24]=bytes(b)
        c=pair_counts(data,0,[0,1,2],25,12,20)
        self.assertEqual(c["samples"],3)
        self.assertEqual(c["sum_exact_255"],3)
        self.assertEqual(c["active_indices_in_range"],2)
        self.assertEqual(c["near_255_and_active_indices_in_range"],2)
        self.assertEqual(c["invalid_only_in_zero_weight_slots"],2)
        self.assertEqual(c["at_least_one_active_index_out_of_range"],1)
        for n in (1,2,3):self.assertEqual(c["nonzero_weight_slots_"+str(n)],1)
        self.assertEqual(c["active_duplicate_indices"],0)
        wrong=pair_counts(data,0,[0,1,2],25,12,24)
        self.assertNotEqual(wrong["near_255_and_active_indices_in_range"],
                            c["near_255_and_active_indices_in_range"])
    def test_reject_invalid_offsets_indices_and_limits(self):
        data=bytes(36)
        for w,i in ((-1,20),(12,36),(12,12),(11,20)):
            with self.assertRaises(ValueError):pair_counts(data,0,[0],25,w,i)
        for joints in (0,65):
            with self.assertRaises(ValueError):pair_counts(data,0,[0],joints,12,20)
        for idx in (-1,1):
            with self.assertRaises(ValueError):pair_counts(data,0,[idx],25,12,20)
        with self.assertRaises(ValueError):
            pair_counts(data,0,list(range(2049)),25,12,20)
        with self.assertRaises(ValueError):stage_scan(b"",0)
        self.assertEqual(len(HYPOTHESES),6)

if __name__=="__main__":unittest.main()
