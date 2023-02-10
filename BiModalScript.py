from collections import defaultdict
import os
import io
import string
import csv
m=7

# Keep the python file in the same directory as your c++ codes with the traces and everything
# Ensure your code only prints misprediction rate

with open("Bimodal.csv",'w')as csvfile:
    fieldnames=['M', 'Mis-prediction rate','Newline']
    filewriter= csv.DictWriter(csvfile, fieldnames=fieldnames)
    while(m<=20):
        cmd = r'./sim bimodal' +" " + str(m) +" " +'gcc_trace.txt'
        test = (os.popen(cmd).read())
        s=test.replace("\n",'')
        mispred_rate= s
        # print((str(m))+ "\t" + str(mispred_rate))
        filewriter.writerow({'M': str(m), 'Mis-prediction rate': str(mispred_rate), 'Newline': ' '})
        m+=1
    filewriter.writerow({'M': ' ', 'Mis-prediction rate': ' ', 'Newline': '\n'})
    #print("\n")

    m=7
    while(m<=20):
        cmd = r'./sim bimodal' +" " + str(m) +" " +'jpeg_trace.txt'
        test = (os.popen(cmd).read())
        s=test.replace("\n",'')
        mispred_rate= s
        # print((str(m))+ "\t" + str(mispred_rate))
        filewriter.writerow({'M': str(m), 'Mis-prediction rate': str(mispred_rate), 'Newline': ' '})
        m+=1
    filewriter.writerow({'M': ' ', 'Mis-prediction rate': ' ', 'Newline': '\n'})
    #print("\n")

    m=7
    while(m<=20):
        cmd = r'./sim bimodal' +" " + str(m) +" " +'perl_trace.txt'
        test = (os.popen(cmd).read())
        s=test.replace("\n",'')
        mispred_rate= s
        # print((str(m))+ "\t" + str(mispred_rate))
        filewriter.writerow({'M': str(m), 'Mis-prediction rate': str(mispred_rate), 'Newline': ' '})
        m+=1
    filewriter.writerow({'M': ' ', 'Mis-prediction rate': ' ', 'Newline': '\n'})