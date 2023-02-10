import os
import io
import string
import csv

# Keep the python file in the same directory as your c++ codes with the traces and everything
# Ensure your code only prints the misprediction rate
m=7
v=7
with open("GShare_gcc_trace.csv",'w')as csvfile:

    fieldnames=['M', 'N', 'Mis-prediction rate']
    filewriter= csv.DictWriter(csvfile, fieldnames=fieldnames)
    while(m<=20):
        n=0
        while(n<=v):
            cmd = r'./sim gshare' +" " + str(m) +" " + str(n)+" "+'gcc_trace.txt'
            test = (os.popen(cmd).read())
            s=test.replace("\n",'')
            mispred_rate= s
            #print((str(m))+ "\t"+str(n)+ "\t" + str(mispred_rate))
            filewriter.writerow({'M': str(m), 'N' :str(n), 'Mis-prediction rate': str(mispred_rate)})
            n+=1
        m+=1
        v+=1
    #print("\n")

m=7
v=7

with open("GShare_jpeg_trace.csv",'w') as csvfile:
    fieldnames =['M', 'N', "Mis-prediction rate"]
    filewriter = csv.DictWriter(csvfile, fieldnames=fieldnames)
    while(m<=20):
        n=0
        while(n<=v):
            cmd = r'./sim gshare' +" " + str(m) +" " +str(n) +" " +'jpeg_trace.txt'
            test = (os.popen(cmd).read())
            s=test.replace("\n",'')
            mispred_rate= s
            #print((str(m))+ "\t" + str(mispred_rate))
            filewriter.writerow({'M': str(m), 'N' :str(n), 'Mis-prediction rate': str(mispred_rate)})
            n+=1
        m+=1
        v+=1
    #print("\n")

m=7
v=7
with open("GShare_perl_trace.csv",'w') as csvfile:
    fieldnames =['M', 'N', "Mis-prediction rate"]
    filewriter = csv.DictWriter(csvfile, fieldnames=fieldnames)
    while(m<=20):
        n=0
        while(n<=v):
            cmd = r'./sim gshare' +" " + str(m) +" " +str(n) +" "+'perl_trace.txt'
            test = (os.popen(cmd).read())
            s=test.replace("\n",'')
            mispred_rate= s
            #print((str(m))+ "\t" + str(mispred_rate))
            filewriter.writerow({'M': str(m), 'N' :str(n), 'Mis-prediction rate': str(mispred_rate)})
            n+=1
        v+=1
        m+=1
