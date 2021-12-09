import os
import subprocess
import re

trace_list = ["int_1.bz2", "int_2.bz2", "fp_1.bz2", "fp_2.bz2",  "mm_1.bz2", "mm_2.bz2"]
predictor_list = ["gshare:13", "tournament:9:10:10", "custom:12:9:9"]


os.system("make clean")
os.system("make")

os.system("echo "" > temp.txt")
for predictor in predictor_list:
    for trace in trace_list:
        string = "bunzip2 -kc ../traces/" + trace + " | ./predictor --" + predictor + " >> temp.txt"
        print string
        #output = subprocess.check_output(string)
        os.system(string)


os.system("grep Mispredic.* temp.txt > temp1.txt")

file = open("temp1.txt" , 'r')

list = []
for val in file :
    list.append(val)
file.close()

file1 = open("result.csv" , 'w')
result = ""
for iter, val in enumerate(list):
    str = val.split(":")[1].strip() 
    if(iter%6==0 and iter!=0):
        #result = result + "\n"
        #result = result + str[1] + "," 
        file1.write("\n")        
    #result = result + str[1] + "," 
    file1.write("%s , " %str) 

file1.close()
#print (result)

