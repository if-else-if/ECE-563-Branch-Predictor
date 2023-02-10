from collections import defaultdict
import os
import io
import string
import csv
m=7
with open("Bimodal.csv",'w')as csvfile:
    filewriter= csv.writer(csvfile, delimiter =' ')
    while(m<=20):
        cmd = r'./sim bimodal' +" " + str(m) +" " +'gcc_trace.txt'
        test = (os.popen(cmd).read())
        s=test.replace("\n",'')
        mispred_rate= s
        print((str(m))+ "\t" + str(mispred_rate))
        filewriter.writerow([str(m), str(mispred_rate)])
        m+=1
    print("\n")

    m=7
    while(m<=20):
        cmd = r'./sim bimodal' +" " + str(m) +" " +'jpeg_trace.txt'
        test = (os.popen(cmd).read())
        s=test.replace("\n",'')
        mispred_rate= s
        print((str(m))+ "\t" + str(mispred_rate))
        filewriter.writerow([str(m), str(mispred_rate)])
        m+=1
    print("\n")

    m=7
    while(m<=20):
        cmd = r'./sim bimodal' +" " + str(m) +" " +'perl_trace.txt'
        test = (os.popen(cmd).read())
        s=test.replace("\n",'')
        mispred_rate= s
        print((str(m))+ "\t" + str(mispred_rate))
        filewriter.writerow([str(m), str(mispred_rate)])
        m+=1