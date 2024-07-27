import argparse

gsymbols = {
    "linecount": 0
}

def fread(path):
    data = None
    with open(path, "r", encoding="utf-8") as file:
        data = file.read()
    return data

def fwrite(data, path):
    with open(path, "w") as file:
        file.write(data)

def debug(data):
    if type(data) == str:
        print(data)
    elif type(data) == list:
        for item in list:
            print(item)

def debugGSymbols():
    print(gsymbols)

def getCommands(chars):
    newlinesCleaned = chars.replace("\n", "")
    lines = newlinesCleaned.split(";")
    return lines[:-1]

def getUnimplementedCommand(line):
    gsymbols[gsymbols["linecount"]] = ["uim", line]
    gsymbols["linecount"] += 1

def getComment(line):
    gsymbols[gsymbols["linecount"]] = ["cmt", line[1:]]
    gsymbols["linecount"] += 1

def symbolize(chars):
    commands = getCommands(chars)
    for command in commands:
        if command[0] == "#":
            getComment(command)
        else:
            getUnimplementedCommand(command)

def translate():
    python = ""
    for commandIndex in range(gsymbols["linecount"]):
        if gsymbols[commandIndex][0] == "cmt":
            python += "#" + gsymbols[commandIndex][1] + "\n"
        else:
            python += gsymbols[commandIndex][1] + "\n"
    return python

def main(source, isTranslate, isInterpret):
    if isInterpret:
        print("Graveyard interpretation is not implemented yet")
        return None

    chars = fread(source)
    symbolize(chars)
    
    # debugGSymbols()

    if isTranslate:
        print("Translating Graveyard to Python...")
        python = translate()
        debug(python)
        # fwrite(python, r"C:\Working\\graveyard\\translated.txt")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source", nargs="?", help="Graveyard source code to operate on.")
    parser.add_argument("-t", "--translate", action="store_true", help="Translate the Graveyard source code.")
    parser.add_argument("-i", "--interpret", action="store_true", help="Interpret the Graveyard source code.")
    args = parser.parse_args()
    main(args.source, args.translate, args.interpret)
