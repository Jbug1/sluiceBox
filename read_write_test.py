#test read file in multiple ways
import time as time
import os


def testy(filepath):

    
    write_start=time.time()
    fileobj=open(filepath, "a") #open filepath for writing to file

    for i in range(int(1e6)):

        fileobj.write("Writing this to a file \n")

    fileobj.close()
    write_end=time.time()-write_start

    write2_start=time.time()

    fileobj=open("1"+filepath, "w") #create a dif filepath for dif writing strat

    outstr=str()

    for i in range(int(1e6)):
        outstr+="Writing this to a file \n"

    fileobj.write(outstr)
    fileobj.close()
    write2_end=time.time()-write2_start

    read_start=time.time()

    fileobj=open(filepath,"r")

    whole_file=fileobj.read()

    line=0
    for i in whole_file:

        if i=="\n":
            line+=1
    
    fileobj.close()
    read_end=time.time()-read_start

    read2_start=time.time()
    fileobj=open(filepath, "r")

    line=0
    while True:

        newline=fileobj.readline()
        line+=1
        if not newline:
            break

    fileobj.close()
    read2_end=time.time()-read2_start

    read3_start=time.time()
    fileobj=open(filepath, "r")

    line=0
    for i in fileobj:
        line+=1

    fileobj.close()
    read3_end=time.time()-read3_start

    return (write_end, write2_end, read_end, read2_end, read3_end)


def main():

    outdict=dict()

    for x in range(5):
        outdict[x]=list()

    
    for i in range(1):

        outs=testy("test.txt")

        os.remove("test.txt")
        os.remove("1"+"test.txt")

        for j in range(len(outs)):
            outdict[j].append(outs[j])
           

    print(f"write1: {sum(outdict[0])/len(outdict[0])}" )
    print(f"write2: {sum(outdict[1])/len(outdict[1])}" )
    print(f"read1: {sum(outdict[2])/len(outdict[2])}" )
    print(f"read2: {sum(outdict[3])/len(outdict[3])}" )
    print(f"read3: {sum(outdict[4])/len(outdict[4])}" )



if __name__ == "__main__":
    main()







