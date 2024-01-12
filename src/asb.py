import argparse
import os.system




class Asbfile():

    def __init__(self):
        self.machine_type = 0
        self.sections = []




if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Convert text to html')
    
    parser.add_argument('filename')                                 # positional argument
    parser.add_argument('-o', '--output')                            # option that takes a value
    #parser.add_argument('-v', '--verbose', action='store_true')     # on/off flag
    args = parser.parse_args()

    f = open(args.filename)
    lines = f.read().split("\n")


    ans = Asbfile()

    i = 0

    while(i < len(lines)):
        l = lines[i]

        x = re.search("^\s*\.machine\s+(\w+)\s*$", l)
        if(x):
            assert x.group(1) == "x86" or x.group(1) == "x64"
            ans.machine_type = (x.group(1) == "64")*1
            continue


        x = re.search("^\s*\.(code|data)section.(\W\w+)\s*$", l)
        if(x):
            sec_name = x.group(2)
            sec_type = (x.group(1) == "code")*1
            sec_asm = []
            while(re.search("\s*\..*$", l) == None and i < len(lines) - 1):
                i += 1
                l = lines[i]
                sec_asm.append(l)

            assert len(sec_name) <= 7
            ans.sections.append([sec_name, sec_type, sec_asm])

            
