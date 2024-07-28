import argparse

gtokens = {
    "linecount": 0,
    "tokencount": 0
}

BASE92 = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "!", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/", ":", ";", "<", "=", ">", "?", "@", "[", "]", "^", "_", "`", "{", "|", "}", "~"]

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
    print("\n")
    if type(data) == str:
        print(data)
    elif type(data) == list:
        for item in list:
            print(item)
    print("\n")

def debugGTokens():
    print(gtokens)

def gGetStatements(chars):
    newlinesCleaned = chars.replace("\n", "")
    lines = newlinesCleaned.split(";")
    return lines[:-1]

def pGetStatements(chars):
    lines = chars.split("\n")
    return lines

def gGetUnimplementedStatement(line):
    gtokens[gtokens["linecount"]] = [UNIMPLEMENTED, line]
    gtokens["linecount"] += 1

def pGetUnimplementedStatement(line):
    gtokens[gtokens["linecount"]] = [UNIMPLEMENTED, line]
    gtokens["linecount"] += 1

def gGetComment(line):
    gtokens[gtokens["linecount"]] = [COMMENT, line[1:]]
    gtokens["linecount"] += 1

def pGetComment(line):
    gtokens[gtokens["linecount"]] = [COMMENT, line[1:]]
    gtokens["linecount"] += 1

def gGetAssignment(operands):
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
    gtokens[gtokens["linecount"]] = assignmentData
    gtokens["linecount"] += 1

def pGetAssignment(operands):
    assignmentData = [ASSIGNMENT]
    for operand in operands:
        cleanedOperand = operand.replace(" ", "")
        if cleanedOperand == "True":
            assignmentData.append("True")
        elif cleanedOperand == "False":
            assignmentData.append("False")
        elif cleanedOperand == "None":
            assignmentData.append("None")
        else:
            assignmentData.append(cleanedOperand)
    gtokens[gtokens["linecount"]] = assignmentData
    gtokens["linecount"] += 1

def tokenizeGraveyard(chars):
    statements = gGetStatements(chars)
    for statement in statements:
        if statement[0] == "#":
            gGetComment(statement)
        else:
            remainingStatementChars = statement
            arbitraryTokenQueue = [""]
            tokenCounter = 0
            statementType = ""
            while len(remainingStatementChars) > 0:
                if remainingStatementChars[0] == " ":
                    remainingStatementChars = remainingStatementChars[1:]
                elif remainingStatementChars[0] == "=":
                    statementType = ASSIGNMENT
                    arbitraryTokenQueue.append("")
                    tokenCounter += 1
                    remainingStatementChars = remainingStatementChars[1:]
                else:
                    arbitraryTokenQueue[tokenCounter] += remainingStatementChars[0]
                    remainingStatementChars = remainingStatementChars[1:]
            if statementType == ASSIGNMENT:
                gGetAssignment(arbitraryTokenQueue)
            else:
                gGetUnimplementedStatement(statement)

def tokenizePython(chars):
    statements = pGetStatements(chars)
    for statement in statements:
        if statement[0] == "#":
            pGetComment(statement)
        else:
            remainingStatementChars = statement
            arbitraryTokenQueue = [""]
            tokenCounter = 0
            statementType = ""
            while len(remainingStatementChars) > 0:
                if remainingStatementChars[0] == "=":
                    statementType = ASSIGNMENT
                    arbitraryTokenQueue.append("")
                    tokenCounter += 1
                    remainingStatementChars = remainingStatementChars[1:]
                else:
                    arbitraryTokenQueue[tokenCounter] += remainingStatementChars[0]
                    remainingStatementChars = remainingStatementChars[1:]
            if statementType == ASSIGNMENT:
                pGetAssignment(arbitraryTokenQueue)
            else:
                pGetUnimplementedStatement(statement)

def translateToTombstone():
    tombstone = ""
    for statementIndex in range(gtokens["linecount"]):
        for token in gtokens[statementIndex]:
            tombstone += token + " "
        tombstone += "\n"
    return tombstone

def translateToPython():
    python = ""
    for statementIndex in range(gtokens["linecount"]):
        if gtokens[statementIndex][0] == COMMENT:
            python += "#" + gtokens[statementIndex][1] + "\n"
        elif gtokens[statementIndex][0] == ASSIGNMENT:
            python += gtokens[statementIndex][1] + " = " + gtokens[statementIndex][2] + "\n"
        else:
            python += gtokens[statementIndex][1] + "\n"
    return python

def translateToNewlinedGraveyard():
    graveyard = ""
    for statementIndex in range(gtokens["linecount"]):
        if gtokens[statementIndex][0] == COMMENT:
            graveyard += "#" + gtokens[statementIndex][1] + ";\n"
        elif gtokens[statementIndex][0] == ASSIGNMENT:
            graveyard += gtokens[statementIndex][1] + "=" + gtokens[statementIndex][2] + ";\n"
        else:
            graveyard += gtokens[statementIndex][1] + ";\n"
    return graveyard

def translateToGraveyard():
    graveyard = ""
    for statementIndex in range(gtokens["linecount"]):
        if gtokens[statementIndex][0] == COMMENT:
            graveyard += "#" + gtokens[statementIndex][1] + ";"
        elif gtokens[statementIndex][0] == ASSIGNMENT:
            graveyard += gtokens[statementIndex][1] + "=" + gtokens[statementIndex][2] + ";"
        else:
            graveyard += gtokens[statementIndex][1] + ";"
    return graveyard

def main(source, isTranslatePython, isTranslateGraveyard, isOutputTombstone, isInterpret, isInterpretTombstone):
    chars = fread(source)

    if isTranslatePython:
        tokenizePython(chars)
        debugGTokens()
        graveyardNewlines = translateToNewlinedGraveyard()
        debug(graveyardNewlines)
        graveyardMinified = translateToGraveyard()
        debug(graveyardMinified)
        fwrite(graveyardNewlines, r"C:\Working\\graveyard\\translatedToGraveyard.txt")
    if isTranslateGraveyard:
        tokenizeGraveyard(chars)
        debugGTokens()
        python = translateToPython()
        debug(python)
        fwrite(python, r"C:\Working\\graveyard\\translatedToPython.txt")
    if isOutputTombstone:
        tombstone = translateToTombstone()
        debug(tombstone)
        fwrite(tombstone, r"C:\Working\\graveyard\\translatedToTombstone.txt")
    if isInterpret:
        print("\nGraveyard interpretation is not implemented yet\n")
    if isInterpretTombstone:
        print("\nTombstone interpretation is not implemented yet\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source", nargs="?", help="Graveyard source code to operate on.")
    parser.add_argument("-tp", "--translatepython", action="store_true", help="Translate the Python source code to Graveyard.")
    parser.add_argument("-tg", "--translategraveyard", action="store_true", help="Translate the Graveyard source code to Python.")
    parser.add_argument("-ot", "--outputtombstone", action="store_true", help="Output the Tombstone representation of the source code.")
    parser.add_argument("-i", "--interpret", action="store_true", help="Interpret the Graveyard source code.")
    parser.add_argument("-it", "--interprettombstone", action="store_true", help="Interpret the raw Tombstone source code.")
    args = parser.parse_args()
    main(args.source, args.translatepython, args.translategraveyard, args.outputtombstone , args.interpret , args.interprettombstone)
