list = range(1,19,1)

file = open("nmn.txt","w")
for iter, val in enumerate(list):
    #str = val.split(":") 
    if(iter%6==0):
        #result = result + "\n"
        #result = result + str[1] + "," 
        file.write("\n")        
    #result = result + str[1] + "," 
    file.write("%s , " %val) 

file.close()
