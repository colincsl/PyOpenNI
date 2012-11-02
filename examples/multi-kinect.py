#!/usr/bin/python
## Can be used for multiple Kinects to  setup depth, image, and users nodes for a depth device from an xml file.

"""
Sets up depth, image, and users nodes for a depth device with an xml file. 
It then reads frames from it, and prints out the middle pixel value.
    
Multiple sensors can be used from seperate Contexts by defining a device
number in the input arguments. Note that this device number starts at index 1.
    
Usage: python multi-kinect.py [devNumber]
ie: python multi-kinect.py 1
"""

import sys
from openni import *

# Get input device number
if len(sys.argv) > 1:
    deviceNumber = sys.argv[1]
else:
    deviceNumber = 1

# Check for available devices
devicesAvailable = ctx.get_device_count()
if deviceNumber > devicesAvailable:
    return

# Init the openni context
ctx = Context()
ctx.init_from_xml_file_by_device_id('SamplesConfig.xml', deviceNumber)

# Get nodes setup in the xml file
depth = ctx.find_existing_node(NODE_TYPE_DEPTH)
color = ctx.find_existing_node(NODE_TYPE_IMAGE)
user = ctx.find_existing_node(NODE_TYPE_USER)

# Start generating
ctx.start_generating_all()

while True:
    # Update to next frame
    nRetVal = ctx.wait_one_update_all(depth)

    depthMap = depth.map

    # Get the coordinates of the middle pixel
    x = depthMap.width / 2
    y = depthMap.height / 2
    
    # Get the pixel at these coordinates
    pixel = depthMap[x,y]

    print "The middle pixel is %d millimeters away." % pixel


