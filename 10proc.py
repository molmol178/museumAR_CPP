#!/usr/bin/python

import subprocess
import pandas as pd
import numpy as np
'''
cmd1 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_p1.png"
cmd2 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_p2.png"
cmd3 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_p3.png"
cmd4 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_p4.png"
cmd5 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_p5.png"
cmd6 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_r1.png"
cmd7 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_r2.png"
cmd8 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_r3.png"
cmd9 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_r4.png"
cmd10 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_s1.png"
cmd10 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_s2.png"
cmd12 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_s3.png"
cmd13 = "./templateinput source_image/experiments/sample.png source_image/experiments/sample_s4.png"

cmd1 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_p1.png ORB'
cmd2 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_p2.png ORB'
cmd3 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_p3.png ORB'
cmd4 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_p4.png ORB'
cmd5 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_p5.png ORB'
cmd6 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_r1.png ORB'
cmd7 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_r2.png ORB'
cmd8 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_r3.png ORB'
cmd9 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_r4.png ORB'
cmd10 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_s1.png ORB'
cmd10 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_s2.png ORB'
cmd12 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_s3.png ORB'
cmd13 = './comparison/comparison source_image/experiments/sample.png source_image/experiments/sample_s4.png ORB'
'''
cmd1 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_p1.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd2 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_p2.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd3 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_p3.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd4 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_p4.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd5 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_p5.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd6 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_r1.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd7 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_r2.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd8 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_r3.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd9 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_r4.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd10 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_s1.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd11 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_s2.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd12 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_s3.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'
cmd13 = './comparison/demo_ASIFT_src/demo_ASIFT source_image/experiments/sample.png source_image/experiments/sample_s4.png imgOutVert.png imgOutHori.png matchings.txt key1.txt key2.txt'

a = np.array([cmd1,cmd2,cmd3,cmd4,cmd5,cmd6,cmd7,cmd8,cmd9,cmd10,cmd11,cmd12,cmd13])
for i in a:
  print(i)
  for j in range(0, 10):
    print(j)
    ret = subprocess.check_output(i.split(" "))
    data = ret.split(",")
    if(j == 0 and i == cmd1):
      df = pd.DataFrame([data])
    else:
      df1 = pd.DataFrame([data])
      df = pd.concat([df, df1])
  #line = pd.DataFrame(["\n"])
  #df = pd.concat([df, line])
df.to_csv('test.csv')
df = pd.read_csv('test.csv')

det1 = df.ix[0:9,2]
det2 = df.ix[10:19,2]
det3 = df.ix[20:29,2]
det4 = df.ix[30:39,2]
det5 = df.ix[40:49,2]
det6 = df.ix[50:59,2]
det7 = df.ix[60:69,2]
det8 = df.ix[70:79,2]
det9 = df.ix[80:89,2]
det10 = df.ix[90:99,2]
det11 = df.ix[100:109,2]
det12 = df.ix[110:119,2]
det13 = df.ix[120:129,2]

det1 = det1.reset_index(drop=True)
det2 = det2.reset_index(drop=True)
det3 = det3.reset_index(drop=True)
det4 = det4.reset_index(drop=True)
det5 = det5.reset_index(drop=True)
det6 = det6.reset_index(drop=True)
det7 = det7.reset_index(drop=True)
det8 = det8.reset_index(drop=True)
det9 = det9.reset_index(drop=True)
det10 = det10.reset_index(drop=True)
det11 = det11.reset_index(drop=True)
det12 = det12.reset_index(drop=True)
det13 = det13.reset_index(drop=True)


detection = pd.concat([det1,det2,det3,det4,det5,det6,det7,det8,det9,det10,det11,det12,det13], axis=1)

desc1 = df.ix[0:9,3]
desc2 = df.ix[10:19,3]
desc3 = df.ix[20:29,3]
desc4 = df.ix[30:39,3]
desc5 = df.ix[40:49,3]
desc6 = df.ix[50:59,3]
desc7 = df.ix[60:69,3]
desc8 = df.ix[70:79,3]
desc9 = df.ix[80:89,3]
desc10 = df.ix[90:99,3]
desc11 = df.ix[100:109,3]
desc12 = df.ix[110:119,3]
desc13 = df.ix[120:129,3]

desc1 = desc1.reset_index(drop=True)
desc2 = desc2.reset_index(drop=True)
desc3 = desc3.reset_index(drop=True)
desc4 = desc4.reset_index(drop=True)
desc5 = desc5.reset_index(drop=True)
desc6 = desc6.reset_index(drop=True)
desc7 = desc7.reset_index(drop=True)
desc8 = desc8.reset_index(drop=True)
desc9 = desc9.reset_index(drop=True)
desc10 = desc10.reset_index(drop=True)
desc11 = desc11.reset_index(drop=True)
desc12 = desc12.reset_index(drop=True)
desc13 = desc13.reset_index(drop=True)



description = pd.concat([desc1,desc2,desc3,desc4,desc5,desc6,desc7,desc8,desc9,desc10,desc11,desc12,desc13], axis=1)


match1 = df.ix[0:9,4]
match2 = df.ix[10:19,4]
match3 = df.ix[20:29,4]
match4 = df.ix[30:39,4]
match5 = df.ix[40:49,4]
match6 = df.ix[50:59,4]
match7 = df.ix[60:69,4]
match8 = df.ix[70:79,4]
match9 = df.ix[80:89,4]
match10 = df.ix[90:99,4]
match11 = df.ix[100:109,4]
match12 = df.ix[110:119,4]
match13 = df.ix[120:129,4]
match1 = match1.reset_index(drop=True)
match2 = match2.reset_index(drop=True)
match3 = match3.reset_index(drop=True)
match4 = match4.reset_index(drop=True)
match5 = match5.reset_index(drop=True)
match6 = match6.reset_index(drop=True)
match7 = match7.reset_index(drop=True)
match8 = match8.reset_index(drop=True)
match9 = match9.reset_index(drop=True)
match10 = match10.reset_index(drop=True)
match11 = match11.reset_index(drop=True)
match12 = match12.reset_index(drop=True)
match13 = match13.reset_index(drop=True)


matching = pd.concat([match1,match2,match3,match4,match5,match6,match7,match8,match9,match10,match11,match12,match13], axis=1)

'''
tot1 = df.ix[0:9,5]
tot2 = df.ix[10:19,5]
tot3 = df.ix[20:29,5]
tot4 = df.ix[30:39,5]
tot5 = df.ix[40:49,5]
tot6 = df.ix[50:59,5]
tot7 = df.ix[60:69,5]
tot8 = df.ix[70:79,5]
tot9 = df.ix[80:89,5]
tot10 = df.ix[90:99,5]
tot11 = df.ix[100:109,5]
tot12 = df.ix[110:119,5]
tot13 = df.ix[120:129,5]

tot1 = tot1.reset_index(drop=True)
tot2 = tot2.reset_index(drop=True)
tot3 = tot3.reset_index(drop=True)
tot4 = tot4.reset_index(drop=True)
tot5 = tot5.reset_index(drop=True)
tot6 = tot6.reset_index(drop=True)
tot7 = tot7.reset_index(drop=True)
tot8 = tot8.reset_index(drop=True)
tot9 = tot9.reset_index(drop=True)
tot10 = tot10.reset_index(drop=True)
tot11 = tot11.reset_index(drop=True)
tot12 = tot12.reset_index(drop=True)
tot13 = tot13.reset_index(drop=True)


total = pd.concat([tot1,tot2,tot3,tot4,tot5,tot6,tot7,tot8,tot9,tot10,tot11,tot12,tot13], axis=1)
'''
lines = pd.DataFrame(["\n"])

detection= detection.reset_index(drop=True)
description = description.reset_index(drop=True)
matching = matching.reset_index(drop=True)
#total = total.reset_index(drop=True)

detection.columns = ['p1', 'p2','p3','p4','p5','r1','r2','r3','r4','s1','s2','s3','s4']
description.columns = ['p1', 'p2','p3','p4','p5','r1','r2','r3','r4','s1','s2','s3','s4']
matching.columns = ['p1', 'p2','p3','p4','p5','r1','r2','r3','r4','s1','s2','s3','s4']
#total.columns = ['p1', 'p2','p3','p4','p5','r1','r2','r3','r4','s1','s2','s3','s4']


all_col = pd.concat([detection,lines,  description,lines,  matching, lines ])


all_col.to_csv("speeds.csv")



