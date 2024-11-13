import sys

sys.path.append("./build")

import example_module as example

obj = example.TeaVis(10)

print(obj.check_normal())

obj.init()
obj.loop()
