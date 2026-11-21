import json,pathlib,tempfile,unittest
from pool_capture import analyze
HEADER={'event':'start','probe':'HOOK001','scope':'read_only_no_multiplayer','reader':'self_process_vm_readv'}
def pool(ms=0,serial=912345,state=0,group=1,reset=1,valid=True,rejected=0):
    r={'event':'pool','ms':ms,'world_epoch':1,'reset_sequence':reset,'valid':valid,'attempts':ms//1000+1,'rejected':rejected}
    if valid:
        r.update(counter=serial+1,occupied=[31,int(group==1),int(group==2),int(group==3)],
                 actors=[[i,group,serial,state,7123456,8123456] if i==31 else [i,0,0,0,0,0] for i in range(32)])
    else:r['rejected']=rejected+1
    return r
class PoolCaptureTests(unittest.TestCase):
    def run_capture(self,records):
        with tempfile.TemporaryDirectory() as d:
            p=pathlib.Path(d)/'capture.txt';p.write_text('\n'.join(json.dumps(r) for r in records)+'\n')
            return analyze(p)
    def test_live_generation_replacement_is_observation_not_spawn(self):
        result=self.run_capture([HEADER,pool(),pool(1000,912346),pool(2000,912346,1,2)])
        self.assertEqual(result['pool']['valid'],3)
        self.assertEqual(result['occupiedSerialChanges'],1)
        self.assertEqual(result['groupTransitions'],{'1->2':1})
        self.assertEqual(result['observedSlotGenerations'],2)
        self.assertFalse(result['captureComplete']);self.assertFalse(result['provesPlayableCoop'])
        text=json.dumps(result)
        for raw in ['7123456','8123456','912345','912346']:self.assertNotIn(raw,text)
    def test_reset_boundary_does_not_misclassify_reused_serial(self):
        result=self.run_capture([HEADER,pool(),pool(1000,1,reset=2)])
        self.assertEqual(result['occupiedSerialChanges'],0)
        self.assertEqual(result['scopeChanges'],1)
    def test_invalid_sample_breaks_transition_chain(self):
        result=self.run_capture([HEADER,pool(),pool(1000,valid=False),pool(2000,912346,1,2,rejected=1)])
        self.assertEqual(result['groupTransitions'],{})
        self.assertEqual(result['occupiedSerialChanges'],0)
        self.assertEqual(result['pool'],{'total':3,'valid':2,'invalid':1})
    def test_duplicate_slot_wrong_counts_and_boolean_integer_rejected(self):
        a=pool();a['actors'][0][0]=31
        b=pool();b['occupied']=[30,2,0,0]
        c=pool();c['actors'][31][2]=True
        for record in [a,b,c]:
            with self.subTest(),self.assertRaises(ValueError):self.run_capture([HEADER,record])
    def test_free_metadata_must_be_zero_and_time_order_checked(self):
        a=pool();a['actors'][0][2]=16
        with self.assertRaises(ValueError):self.run_capture([HEADER,a])
        with self.assertRaises(ValueError):self.run_capture([HEADER,pool(1000),pool(0)])
    def test_header_missing_or_non_probe_capture_rejected(self):
        with self.assertRaises(ValueError):self.run_capture([pool()])
        with self.assertRaises(ValueError):self.run_capture([dict(HEADER,scope='multiplayer'),pool()])
    def test_end_marks_completion_without_exposing_legacy_positions(self):
        result=self.run_capture([HEADER,{'event':'player','position':[123456789,2,3],'thread':9912345},pool(),{'event':'end','records':2}])
        self.assertTrue(result['captureComplete'])
        self.assertNotIn('123456789',json.dumps(result));self.assertNotIn('9912345',json.dumps(result))
if __name__=='__main__':unittest.main()
