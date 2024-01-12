import argparse
import re


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Convert text to html')
    
    parser.add_argument('filename') #filename of list of instructions
    args = parser.parse_args()


    f = open(args.filename, 'r')
    lines = (f.read()).split("\n")

    inst = []

    prev = ""
    for l in lines:
        if(not re.search('.*[01][01].*', l)):
            prev = l
        else:
            x = re.search('^(([^01]*([01]|)[^01]*) - ([^01]*([01]|)[^01]*)) ([01][01].*)$', l)
            if(x):
                bit_string = x.group(6)
                name = x.group(1)
                prev = name
                inst.append([name, "", bit_string])
            else:
                x = re.search('^([^01]*([01]|)[^01]*) ([01][01].*)$', l)
                if(not x):
                    print("Couldnt match " + l)
                    exit()
                inst.append([prev, x.group(1), x.group(3)])


    # i in inst: [ADD, ADD - details, btyes]
    # inst in format :: [Numonic, Desciption, case format] 
    s = '''
typedef struct{
    char *numonic;
    char *descrip;
    char *case;
    char *format;
} instruction_entry_t;
'''
    ans = ""
    ans += "static instruction_entry_t instructions[] = { \n"
    j = 0    
    for i in inst:

        x = re.search("^([^-/]*)( -|/)( |)(.*)$", i[0])
        ans += x.group(1) + ";" + x.group(4) + ";" + i[1] + ";" + i[2].replace(" ", "") + "\n"
        j += 1
        continue
        ans += "{"
        ans += "\"" + x.group(1) + "\", "
        ans += "\"" + x.group(4) + "\", "
        ans += "\"" + i[1] + "\", "
        ans += "\"" + i[2].replace(" ", "") + "\""
        ans += "}, \n"
        j += 1
    
    ans = s + "\n\n" + "int n_instructions = " + str(j) + ";\n" + ans[:-3] +"};"
    
    print(ans)