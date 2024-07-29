import argparse
import re

gtokens = {
    "linecount": 0,
    "tokenmap": []
}

BASE92 = "~ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&'()*+,-./:;<=>?@[]^_`{|}"

UNIMPLEMENTED = "uim"
COMMENT = "cmt"
ASSIGNMENT = "asn"

def base92(decimal):
    base92String = ""
    if decimal >= 0:
        while True:
            base92String = f"{BASE92[decimal % 92]}{base92String}"
            decimal //= 92
            if decimal <= 0:
                break
        return base92String
    else:
        print("cannot convert negative decimal to base92")
        return None

def fromBase92(base92):
    counter = 0
    for digit in BASE92:
        if digit == base92:
            return(counter)
        else:
            counter += 1

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
        elif operand[0] == ":":
            mappedToken = operand[1:-1]
            assignmentData.append(gtokens["tokenmap"][fromBase92(mappedToken) - 1])
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
        elif cleanedOperand.startswith('"'):
            assignmentData.append(cleanedOperand)
        elif re.match(r"^[\d\.]+$", cleanedOperand):
            assignmentData.append(cleanedOperand)
        else:
            if cleanedOperand not in gtokens["tokenmap"]:
                gtokens["tokenmap"].append(cleanedOperand)
            assignmentData.append(cleanedOperand)

    gtokens[gtokens["linecount"]] = assignmentData
    gtokens["linecount"] += 1

def tokenizeGraveyard(chars):
    statements = gGetStatements(chars)
    if statements[0].startswith("::"):
        mappedTokens = statements[0][2:].split(":")
        for mappedToken in mappedTokens:
            gtokens["tokenmap"].append(mappedToken)
    statements = statements[1:]
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
    for mappedToken in gtokens["tokenmap"]:
        tombstone += f"tkn {mappedToken}\n"
    tombstone += "sta\n"
    for statementIndex in range(gtokens["linecount"]):
        for token in gtokens[statementIndex]:
            tombstone += f"{token} "
        tombstone += "\n"
    tombstone += "end\n"
    return tombstone

def translateToPython():
    python = ""
    for statementIndex in range(gtokens["linecount"]):
        if gtokens[statementIndex][0] == COMMENT:
            python += f"#{gtokens[statementIndex][1]}\n"
        elif gtokens[statementIndex][0] == ASSIGNMENT:
            python += f"{gtokens[statementIndex][1]} = {gtokens[statementIndex][2]}\n"
        else:
            python += f"{gtokens[statementIndex][1]}\n"
    return python

def translateToNewlinedGraveyard():
    graveyard = ":"
    for mappedToken in gtokens["tokenmap"]:
        graveyard += f":{mappedToken}"
    graveyard += ";\n"
    for statementIndex in range(gtokens["linecount"]):
        if gtokens[statementIndex][0] == COMMENT:
            graveyard += f"#{gtokens[statementIndex][1]};\n"
        elif gtokens[statementIndex][0] == ASSIGNMENT:
            left = gtokens[statementIndex][1]
            leftCounter = 0
            for mappedToken in gtokens["tokenmap"]:
                if left == mappedToken:
                    left = f":{base92(leftCounter + 1)}:"
                else:
                    leftCounter += 1
            right = gtokens[statementIndex][2]
            rightCounter = 0
            for mappedToken in gtokens["tokenmap"]:
                if right == mappedToken:
                    right = f":{base92(rightCounter + 1)}:"
                else:
                    rightCounter += 1
            graveyard += f"{left}={right};\n"
        else:
            graveyard += f"{gtokens[statementIndex][1]};\n"
    return graveyard

def translateToGraveyard():
    graveyard = ":"
    for mappedToken in gtokens["tokenmap"]:
        graveyard += f":{mappedToken}"
    graveyard += ";"
    for statementIndex in range(gtokens["linecount"]):
        if gtokens[statementIndex][0] == COMMENT:
            graveyard += f"#{gtokens[statementIndex][1]};"
        elif gtokens[statementIndex][0] == ASSIGNMENT:
            left = gtokens[statementIndex][1]
            leftCounter = 0
            for mappedToken in gtokens["tokenmap"]:
                if left == mappedToken:
                    left = f":{base92(leftCounter + 1)}:"
                else:
                    leftCounter += 1
            right = gtokens[statementIndex][2]
            rightCounter = 0
            for mappedToken in gtokens["tokenmap"]:
                if right == mappedToken:
                    right = f":{base92(rightCounter + 1)}:"
                else:
                    rightCounter += 1
            graveyard += f"{left}={right};"
        else:
            graveyard += f"{gtokens[statementIndex][1]};"
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
        # fwrite(graveyardNewlines, r"C:\Working\\graveyard\\translatedToGraveyard.txt")
    if isTranslateGraveyard:
        tokenizeGraveyard(chars)
        debugGTokens()
        python = translateToPython()
        debug(python)
        # fwrite(python, r"C:\Working\\graveyard\\translatedToPython.txt")
    if isOutputTombstone:
        tombstone = translateToTombstone()
        debug(tombstone)
        # fwrite(tombstone, r"C:\Working\\graveyard\\translatedToTombstone.txt")
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

# !* fix fromBase92() algorithm to work with numbers over 91
# !* allow robust token parsing when graveyard code has a mix of mapped and unmapped tokens
# !* modularize translateToGraveyard functions because there's a lot of repeated code there