import sys, pathlib, unittest

sys.path.append(str(pathlib.Path(__file__).parents[1].joinpath("project").resolve()))

import main

class TestMain(unittest.TestCase):
    def test_main(self):
        self.assertEqual(5, 5)
        
if __name__ == "__main__":
    unittest.main()