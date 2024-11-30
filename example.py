import sys

sys.path.append("./build")

from teavoxelui import TeaVis

obj = TeaVis(10)

print(obj.check_normal())

obj.init()
obj.loop()
