#!/usr/bin/python
import math

numOfCameras = 6

for i in range(numOfCameras):
    print "camera " + str(i+1)

    print "\"origin\" : [ "
    print str(9.6*math.cos( i * ( (2*math.pi)/numOfCameras ) )) + ","
    print str(-9.6*math.sin( i * ( (2*math.pi)/numOfCameras ) )) + ","

    # print "\"right\" : [ "
    # print str(-math.sin( i * ( (2*math.pi)/numOfCameras ) )) + ","
    # print str(-math.cos( i * ( (2*math.pi)/numOfCameras ) )) + ","


    # print "\"forward\" : [ "
    # print str(math.cos( i * ( (2*math.pi)/numOfCameras ) )) + ","
    # print str(-math.sin( i * ( (2*math.pi)/numOfCameras ) )) + ","
    print "\n"
