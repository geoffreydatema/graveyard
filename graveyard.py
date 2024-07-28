import argparse

gtokens = {
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

def main(source, isTranslate, isInterpret):
    if isInterpret:
        print("\nGraveyard interpretation is not implemented yet\n")
        return None

    chars = fread(source)
    tokenizeGraveyard(chars)
    # tokenizePython(chars)
    
    debugGTokens()

    if isTranslate:
        print("")
        python = translateToPython()
        tombstone = translateToTombstone()
        graveyardNewlines = translateToNewlinedGraveyard()
        graveyardMinified = translateToGraveyard()
        debug(python)
        print("---\n")
        debug(tombstone)
        print("---\n")
        debug(graveyardNewlines)
        print("---\n")
        debug(graveyardMinified)
        # fwrite(python, r"C:\Working\\graveyard\\translatedToPython.txt")
        # fwrite(tombstone, r"C:\Working\\graveyard\\translatedToTombstone.txt")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("source", nargs="?", help="Graveyard source code to operate on.")
    parser.add_argument("-t", "--translate", action="store_true", help="Translate the Graveyard source code.")
    parser.add_argument("-i", "--interpret", action="store_true", help="Interpret the Graveyard source code.")
    args = parser.parse_args()
    main(args.source, args.translate, args.interpret)

# !* token compression for extra minification