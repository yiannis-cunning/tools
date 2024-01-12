import argparse
import re

def passert(bol, msg):
    if( not bol):
        print(msg)
        exit(1)

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='takes in the dtb in the form of ADD;DESCP;CASE;OPCODE')
    parser.add_argument('filename') #filename of list of instructions
    args = parser.parse_args()
    f = open(args.filename, 'r')
    
    
    lines = (f.read()).split("\n")
    #print(lines)
    # 1) ADD    : name
    # 2)        : descp
    # 2) ADD R1 immediate to t1 : case
    # 3) 010101... 

    # want to add asertions about the parsed files...
    for l in lines:
        i = l.split(";")

        # (all description feilds)
        exp = "(\w|\s|-|[0-9])+"
        passert(re.search(exp, i[0]) != None, "Name does is not normal " + i[0])
        passert(re.search(exp, i[1]) != None, "Description is not normal " + i[1])
        exp =  "(\w|\s|-|[0-9])*"
        passert(re.search(exp, i[2]) != None, "Case is not normal " + i[2])

        exp = "(0|1|s|w){8}(|:.*)$"
        passert(re.search(exp, i[3]) != None, "Opcode not normal " + i[3])

    print("Checked all lines")
        