import argparse

gsymbols = {
    "linecount": 0
}

UNIMPLEMENTED = "uim"
COMMENT = "cmt"
ASSIGNMENT = "asn"

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

def getStatements(chars):
    newlinesCleaned = chars.replace("\n", "")
    lines = newlinesCleaned.split(";")
    return lines[:-1]

def getUnimplementedStatement(line):
    gsymbols[gsymbols["linecount"]] = [UNIMPLEMENTED, line]
    gsymbols["linecount"] += 1

def getComment(line):
    gsymbols[gsymbols["linecount"]] = [COMMENT, line[1:]]
    gsymbols["linecount"] += 1

def getAssignment(operands):
    assignmentData = [ASSIGNMENT]
    for operand in operands:
        if operand == "$":
            assignmentData.append("True")
        elif operand == ":":
            assignmentData.append("False")
        elif operand == "@":
            assignmentData.append("None")
        else:
            assignmentData.append(operand)
    gsymbols[gsymbols["linecount"]] = assignmentData
    gsymbols["linecount"] += 1

def symbolize(chars):
    statements = getStatements(chars)
    for statement in statements:
        if statement[0] == "#":
            getComment(statement)
        else:
            remainingStatementChars = statement
            arbitrarySymbolQueue = [""]
            symbolCounter = 0
            statementType = ""
            while len(remainingStatementChars) > 0:
                if remainingStatementChars[0] == " ":
                    remainingStatementChars = remainingStatementChars[1:]
                elif remainingStatementChars[0] == "=":
                    statementType = ASSIGNMENT
                    arbitrarySymbolQueue.append("")
                    symbolCounter += 1
                    remainingStatementChars = remainingStatementChars[1:]
                else:
                    arbitrarySymbolQueue[symbolCounter] += remainingStatementChars[0]
                    remainingStatementChars = remainingStatementChars[1:]
            if statementType == ASSIGNMENT:
                getAssignment(arbitrarySymbolQueue)
            else:
                getUnimplementedStatement(statement)

def translateToTombstone():
    tombstone = ""
    for statementIndex in range(gsymbols["linecount"]):
        for symbol in gsymbols[statementIndex]:
            tombstone += symbol + " "
        tombstone += "\n"
    return tombstone

def translateToPython():
    python = ""
    for statementIndex in range(gsymbols["linecount"]):
        if gsymbols[statementIndex][0] == COMMENT:
            python += "#" + gsymbols[statementIndex][1] + "\n"
        elif gsymbols[statementIndex][0] == ASSIGNMENT:
            python += gsymbols[statementIndex][1] + " = " + gsymbols[statementIndex][2] + "\n"
        else:
            python += gsymbols[statementIndex][1] + "\n"
    return python

def main(source, isTranslate, isInterpret):
    if isInterpret:
        print("\nGraveyard interpretation is not implemented yet\n")
        return None

    chars = fread(source)
    symbolize(chars)
    
    debugGSymbols()

    if isTranslate:
        print("")
        python = translateToPython()
        tombstone = translateToTombstone()
        debug(python)
        print("---\n")
        debug(tombstone)
        fwrite(python, r"C:\Working\\graveyard\\translatedToPython.txt")
        fwrite(tombstone, r"C:\Working\\graveyard\\translatedToTombstone.txt")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source", nargs="?", help="Graveyard source code to operate on.")
    parser.add_argument("-t", "--translate", action="store_true", help="Translate the Graveyard source code.")
    parser.add_argument("-i", "--interpret", action="store_true", help="Interpret the Graveyard source code.")
    args = parser.parse_args()
    main(args.source, args.translate, args.interpret)
